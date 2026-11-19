/* mfl_trap.c - release exe entry that reports a crash by function name (tools/delink/link_release.sh TRAP=1, /entry:mfl_release_entry).
 *
 * mfl_release_entry installs an unhandled-exception filter and then runs the CRT start-up (WinMainCRTStartup).  The filter takes the code and
 * address of the fault (0x80000003 = an int3 from a rw_trap.txt row, 0xC0000005 = access violation, ...), looks the address up in
 * MFL.map beside the exe (nearest `name` with VA <= address), writes "trap <code> at <addr> in <name>+<off>" to mfl_trap.log beside the
 * exe, shows a MessageBox with the same text (not when the env MFL_TRAP_NOBOX is "1") and ends the process with exit code 3.
 * It changes no trap list: it only reports what faults.  Not in any gate build (recomp/release/ is not globbed by build_overlay.py).
 *
 * cl 13 / standalone link rules (see recomp/video/mfl_video_null.c): no string literals in code (every string is a static const array),
 * only zero-initialised statics, no CRT (hex formatting by hand), only LoadLibraryA / GetModuleHandleA / GetProcAddress imported
 * directly (the import libs exist only for names the original exe imports); everything else comes through GetProcAddress.
 * ponytail: the map is scanned linearly on the first fault (64 KiB chunks, no cache); fine for a one-shot crash report. */
typedef unsigned long u32;

extern __declspec(dllimport) void *__stdcall LoadLibraryA(const char *name);
extern __declspec(dllimport) void *__stdcall GetModuleHandleA(const char *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
/* The CRT start-up: libcmt's wincrt0.obj with LIB_CRT=libcmt, else the original entry (recomp/aliases.txt names it for the overlay). */
extern void __cdecl WinMainCRTStartup(void);

struct xrec { u32 code; u32 flags; struct xrec *next; void *addr; };
struct xptrs { struct xrec *rec; void *ctx; };

typedef void *(__stdcall *createfile_fn)(const char *name, u32 access, u32 share, void *sa, u32 disp, u32 flags, void *tmpl);
typedef int (__stdcall *readfile_fn)(void *h, void *buf, u32 n, u32 *got, void *ov);
typedef int (__stdcall *writefile_fn)(void *h, const void *buf, u32 n, u32 *done, void *ov);
typedef int (__stdcall *closehandle_fn)(void *h);
typedef u32 (__stdcall *getmodname_fn)(void *mod, char *buf, u32 n);
typedef u32 (__stdcall *getenv_fn)(const char *name, char *buf, u32 n);
typedef void (__stdcall *exitproc_fn)(u32 code);
typedef long (__stdcall *filter_fn)(struct xptrs *p);
typedef filter_fn (__stdcall *setfilter_fn)(filter_fn f);
typedef int (__stdcall *msgbox_fn)(void *wnd, const char *text, const char *caption, u32 type);

static const char k_kernel32[] = "kernel32.dll";
static const char k_user32[] = "user32.dll";
static const char k_setfilter[] = "SetUnhandledExceptionFilter";
static const char k_createfile[] = "CreateFileA";
static const char k_readfile[] = "ReadFile";
static const char k_writefile[] = "WriteFile";
static const char k_closehandle[] = "CloseHandle";
static const char k_getmodname[] = "GetModuleFileNameA";
static const char k_getenv[] = "GetEnvironmentVariableA";
static const char k_exitproc[] = "ExitProcess";
static const char k_msgbox[] = "MessageBoxA";
static const char k_nobox[] = "MFL_TRAP_NOBOX";
static const char k_logname[] = "mfl_trap.log";
static const char k_caption[] = "MFL trap";
static const char k_s_trap[] = "trap 0x";
static const char k_s_at[] = " at 0x";
static const char k_s_in[] = " in ";
static const char k_s_off[] = "+0x";
static const char k_s_unknown[] = "?";
static const char k_s_nl[] = "\r\n";
static const char k_hex[] = "0123456789abcdef";

#define CHUNK 65536
static char g_buf[CHUNK + 512];                /* map chunk + carried partial line */
static char g_path[300];
static char g_name[96];
static char g_text[300];
static u32 g_best;                             /* VA of the nearest symbol so far (0 = none) */
static u32 g_addr;

static char *app(char *d, const char *s) { while (*s) *d++ = *s++; return d; }
static char *app_hex(char *d, u32 v) { int i; for (i = 28; i >= 0; i -= 4) *d++ = k_hex[(v >> i) & 15]; return d; }

static int is_hex(char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

/* one map line: "<seg>:<off>  <name>  <va hex>  [f]  <obj>"; remember the symbol if it is the nearest one at or below g_addr */
static void scan_line(const char *l)
{
    const char *n, *e;
    u32 va = 0;
    int digits = 0, i;
    while (*l == ' ') l++;
    for (e = l; *e && *e != ' '; e++) if (*e == ':') break;
    if (*e != ':') return;
    while (*l && *l != ' ') l++;
    while (*l == ' ') l++;
    n = l;                                     /* symbol name */
    while (*l && *l != ' ') l++;
    e = l;
    while (*l == ' ') l++;
    while (is_hex(*l)) { va = va * 16 + (u32)(*l <= '9' ? *l - '0' : (*l | 32) - 'a' + 10); l++; digits++; }
    if (digits < 6 || (*l != ' ' && *l != 0)) return;
    if (va <= g_addr && va > g_best) {
        g_best = va;
        for (i = 0; n + i < e && i < (int)sizeof g_name - 1; i++) g_name[i] = n[i];
        g_name[i] = 0;
    }
}

static void lookup(void *k32, u32 addr)
{
    createfile_fn createfile = (createfile_fn)GetProcAddress(k32, k_createfile);
    readfile_fn readfile = (readfile_fn)GetProcAddress(k32, k_readfile);
    closehandle_fn closeh = (closehandle_fn)GetProcAddress(k32, k_closehandle);
    void *h;
    u32 got, carry = 0, len;
    g_addr = addr;
    g_best = 0;
    g_name[0] = 0;
    len = ((getmodname_fn)GetProcAddress(k32, k_getmodname))(0, g_path, sizeof g_path - 16);
    if (!len || !createfile || !readfile || !closeh) return;
    g_path[len - 3] = 'm'; g_path[len - 2] = 'a'; g_path[len - 1] = 'p';     /* MFL.exe -> MFL.map */
    h = createfile(g_path, 0x80000000UL, 1, 0, 3, 0, 0);                      /* GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING */
    if (h == (void *)-1) return;
    while (readfile(h, g_buf + carry, CHUNK, &got, 0) && got) {
        u32 end = carry + got, s = 0, i, last = 0;
        for (i = 0; i < end; i++) if (g_buf[i] == '\n') last = i + 1;
        for (i = 0; i < last; i++) if (g_buf[i] == '\n' || g_buf[i] == '\r') { g_buf[i] = 0; scan_line(g_buf + s); s = i + 1; }
        carry = end - last;
        if (carry > 500) carry = 0;            /* not a map line */
        for (i = 0; i < carry; i++) g_buf[i] = g_buf[last + i];
    }
    closeh(h);
}

static long __stdcall trap_filter(struct xptrs *p)
{
    void *k32 = GetModuleHandleA(k_kernel32);
    char *d, box[8];
    u32 code = p->rec->code, addr = (u32)p->rec->addr, len, done;
    void *h;
    lookup(k32, addr);
    d = g_text;
    d = app(d, k_s_trap); d = app_hex(d, code);
    d = app(d, k_s_at); d = app_hex(d, addr);
    d = app(d, k_s_in);
    d = app(d, g_best ? g_name : k_s_unknown);
    if (g_best) { d = app(d, k_s_off); d = app_hex(d, addr - g_best); }
    *d = 0;
    len = (u32)(d - g_text);
    /* log beside the exe: reuse g_path, cut at the last backslash */
    {
        u32 n = 0, i;
        for (i = 0; g_path[i]; i++) if (g_path[i] == '\\' || g_path[i] == '/') n = i + 1;
        *app(g_path + n, k_logname) = 0;
        h = ((createfile_fn)GetProcAddress(k32, k_createfile))(g_path, 0x40000000UL, 1, 0, 2, 0, 0);   /* GENERIC_WRITE, CREATE_ALWAYS */
        if (h != (void *)-1 && h) {
            ((writefile_fn)GetProcAddress(k32, k_writefile))(h, g_text, len, &done, 0);
            ((writefile_fn)GetProcAddress(k32, k_writefile))(h, k_s_nl, 2, &done, 0);
            ((closehandle_fn)GetProcAddress(k32, k_closehandle))(h);
        }
    }
    box[0] = 0;
    len = ((getenv_fn)GetProcAddress(k32, k_getenv))(k_nobox, box, sizeof box);
    if (!(len == 1 && box[0] == '1')) {
        void *u32m = LoadLibraryA(k_user32);
        msgbox_fn mb = u32m ? (msgbox_fn)GetProcAddress(u32m, k_msgbox) : 0;
        if (mb) mb(0, g_text, k_caption, 0x10);   /* MB_ICONERROR */
    }
    ((exitproc_fn)GetProcAddress(k32, k_exitproc))(3);
    return 1;
}

void mfl_release_entry(void)
{
    setfilter_fn set = (setfilter_fn)GetProcAddress(GetModuleHandleA(k_kernel32), k_setfilter);
    if (set) set(trap_filter);
    WinMainCRTStartup();
}
