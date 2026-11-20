/* mfl_crashlog.c: crash / abnormal-exit reporter of the v143 release exe (platform layer, not an original function; MFL_FIX_CRASHLOG).
 * A silent close on Windows (user report W16: Escape on Windows 11) leaves no trace; with the switch on, every abnormal end appends one
 * report to mflng-crash.txt.  Folder: the current directory, where the game itself writes gamesave.bin, mashed.log and videocfg.bin
 * (relative names, batch4.c / log_open_write.c); if that cannot be written, the exe's folder, then %TEMP%.  The file is kept below 64 KB:
 * a bigger one is renamed to mflng-crash.old.txt once (replacing the older one).
 * Caught: an unhandled exception in any thread (SetUnhandledExceptionFilter, chained to the filter before ours); stack overflow, stack
 * buffer overrun, heap corruption (0xc0000374, which ntdll hands to WER without the unhandled filter) and fatal app exit already at first
 * chance (vectored handler); a UCRT
 * invalid-parameter stop, a pure virtual call, abort() (SIGABRT); exit / _exit / ExitProcess / TerminateProcess with a code other than 0,
 * through their slots in our own import table.  A normal quit (WinMain returns 0, exit(0)) writes nothing.  After the report every path
 * ends as it would without this file (the previous filter, the CRT's fail-fast, the real exit), so Windows Error Reporting still sees it.
 * Not caught: __fastfail (int 0x29) never reaches user mode handlers on Windows 8+ (only WER / Event Viewer shows it).
 * Report: UTC time, the exe's PE time stamp (= "Timestamp is" in its .map), reason, code, address as module+offset (the exe is /fixed
 * at 0x400000: the address itself is the .map VA), registers, 32 stack words, the EBP chain and the return addresses found on the stack
 * (the game code is /O2 = frame pointers omitted: the EBP chain stops early, the stack scan is the useful one), race phase / menu / app
 * state words, the last first-chance access violation (or illegal instruction, divide) seen before, thread id, the process heap mode read at start-up (2 = LFH), command line, Windows build.
 * Rules inside a report: static buffers only, no CRT call, kernel32 only (CreateFileW / WriteFile), at most one report per process.
 * Built only by MSVC v143 (VC7 cl 13 = 1300 never compiles it; build_overlay.py leaves the file out of a build without the switch). */
#include "mfl_fix.h"
#if _MSC_VER >= 1900 && MFL_FIX_CRASHLOG
#include <signal.h>
#include <stdlib.h>
#include <intrin.h>

typedef unsigned long u32;
struct xrec { u32 code, flags; struct xrec *next; void *addr; };
struct xptrs { struct xrec *rec; u32 *ctx; };      /* ctx = x86 CONTEXT, read by the offsets below */
struct meminfo { void *base, *alloc_base; u32 alloc_protect, size, state, protect, type; };
struct systime { unsigned short year, month, weekday, day, hour, minute, second, ms; };
struct osver { u32 size, major, minor, build, platform; wchar_t sp[128]; };
typedef long (__stdcall *filter_fn)(struct xptrs *p);

extern __declspec(dllimport) filter_fn __stdcall SetUnhandledExceptionFilter(filter_fn f);
extern __declspec(dllimport) void *__stdcall AddVectoredExceptionHandler(u32 first, filter_fn f);
extern __declspec(dllimport) int __stdcall SetThreadStackGuarantee(u32 *size);
extern __declspec(dllimport) void *__stdcall CreateFileW(const wchar_t *path, u32 access, u32 share, void *sa, u32 disp, u32 flags, void *tmpl);
extern __declspec(dllimport) int __stdcall WriteFile(void *h, const void *data, u32 n, u32 *done, void *ov);
extern __declspec(dllimport) int __stdcall CloseHandle(void *h);
extern __declspec(dllimport) u32 __stdcall GetFileSize(void *h, u32 *high);
extern __declspec(dllimport) int __stdcall MoveFileExW(const wchar_t *from, const wchar_t *to, u32 flags);
extern __declspec(dllimport) u32 __stdcall GetModuleFileNameW(void *module, wchar_t *path, u32 size);
extern __declspec(dllimport) u32 __stdcall GetTempPathW(u32 size, wchar_t *path);
extern __declspec(dllimport) void *__stdcall GetModuleHandleW(const wchar_t *name);
extern __declspec(dllimport) void *__stdcall GetProcAddress(void *module, const char *name);
extern __declspec(dllimport) u32 __stdcall VirtualQuery(const void *at, struct meminfo *mi, u32 size);
extern __declspec(dllimport) int __stdcall VirtualProtect(void *at, u32 size, u32 protect, u32 *old);
extern __declspec(dllimport) void __stdcall GetSystemTime(struct systime *t);
extern __declspec(dllimport) char *__stdcall GetCommandLineA(void);
extern __declspec(dllimport) u32 __stdcall GetCurrentThreadId(void);
extern __declspec(dllimport) void *__stdcall GetProcessHeap(void);
extern __declspec(dllimport) int __stdcall HeapQueryInformation(void *heap, u32 kind, void *info, u32 size, u32 *got);
extern __declspec(dllimport) void __stdcall ExitProcess(u32 code);
extern __declspec(dllimport) int __stdcall TerminateProcess(void *process, u32 code);

/*00652f68*/ extern int g_race_phase;      /* 6 = race running (tools/delink/race_relinked.sh) */
/*0069cbd0*/ extern int g_menu_current;      /* menu depth (tools/ci/smoke_release.py) */
/*0076e7f4*/ extern int g_app_state;

/* x86 CONTEXT dword indexes (winnt.h offsets / 4) */
enum { C_EDI = 0x9c / 4, C_ESI, C_EBX, C_EDX, C_ECX, C_EAX, C_EBP, C_EIP, C_CS, C_EFL, C_ESP, C_SS, CONTEXT_WORDS = 0x2cc / 4 };
#define MAX_REPORT_FILE 60000UL      /* rotate above this; one report is < 4 KB, so the file stays < 64 KB */

static char g_out[8192];
static char *g_p;
static wchar_t g_path[300], g_old[300];
static u32 g_ctx[CONTEXT_WORDS];
static u32 g_heap_mode, g_heap_known;      /* process heap at start-up: 0 standard, 2 low-fragmentation heap (LFH); zero-initialised only (.bss) */
static volatile long g_busy;     /* 1 = a report was written (or is being written): one per process */
static volatile u32 g_fc_code, g_fc_addr, g_fc_count;      /* last first-chance access violation / illegal instruction / divide */
static filter_fn g_prev_filter;
static void (__cdecl *g_exit)(int), (__cdecl *g_exit2)(int);
static void (__stdcall *g_exitprocess)(u32);
static int (__stdcall *g_terminate)(void *, u32);

static void s(const char *t) { while (*t && g_p < g_out + sizeof g_out - 1) *g_p++ = *t++; }
static void hex(u32 v) { char b[11]; int i; b[0] = '0'; b[1] = 'x'; for (i = 0; i < 8; i++) b[2 + i] = "0123456789abcdef"[(v >> (28 - 4 * i)) & 15]; b[10] = 0; s(b); }
static void dec(u32 v) { char b[12]; int i = 11; b[i] = 0; do b[--i] = (char)('0' + v % 10); while (v /= 10); s(b + i); }
static void dec2(u32 v) { if (v < 10) s("0"); dec(v); }
static void ws(const wchar_t *t, int max) { char c[2] = { 0, 0 }; for (; *t && max-- > 0; t++) { c[0] = *t < 128 ? (char)*t : '?'; s(c); } }

/* "0x00401234 MFLng.exe+0x1234" (module from the memory region, so no loader lock) */
static void addr(u32 a)
{
  struct meminfo mi;
  wchar_t *name, *q;
  hex(a);
  if (VirtualQuery((void *)a, &mi, sizeof mi) == 0 || mi.type != 0x1000000 /* MEM_IMAGE */) return;
  if (GetModuleFileNameW(mi.alloc_base, g_old, 300) == 0) return;
  for (name = q = g_old; *q; q++) if (*q == '\\' || *q == '/') name = q + 1;
  s(" "); ws(name, 40); s("+"); hex(a - (u32)mi.alloc_base);
}

static int stack_ok(u32 at) { return at >= __readfsdword(8) && at + 4 <= __readfsdword(4) && (at & 3) == 0; }      /* TEB StackLimit / StackBase */

static int is_return_address(u32 v)      /* in an executable image region and right after a call instruction */
{
  struct meminfo mi;
  const unsigned char *b = (const unsigned char *)v;
  if (v < 0x10000 || VirtualQuery(b - 8, &mi, sizeof mi) == 0 || mi.type != 0x1000000 || !(mi.protect & 0xf0) || (u32)mi.base + mi.size < v) return 0;
  return b[-5] == 0xe8 || (b[-2] == 0xff && (b[-1] & 0x38) == 0x10) || (b[-3] == 0xff && (b[-2] & 0x38) == 0x10) || (b[-6] == 0xff && (b[-5] & 0x38) == 0x10)
         || (b[-7] == 0xff && (b[-6] & 0x38) == 0x10);
}

/* "mflng-crash.txt" in dir[0..n): open for append, rotate once when big */
static void *open_in(u32 n)
{
  static const wchar_t name[] = L"mflng-crash.txt", old[] = L"mflng-crash.old.txt";
  void *h;
  u32 i;
  for (i = 0; old[i]; i++) g_old[n + i] = old[i];
  g_old[n + i] = 0;
  for (i = 0; name[i]; i++) g_path[n + i] = name[i];
  g_path[n + i] = 0;
  /* FILE_APPEND_DATA | FILE_READ_ATTRIBUTES | SYNCHRONIZE, share read, OPEN_ALWAYS */
  h = CreateFileW(g_path, 0x100084UL, 1, 0, 4, 0x80, 0);
  if (h != (void *)-1 && GetFileSize(h, 0) > MAX_REPORT_FILE) {
    CloseHandle(h);
    MoveFileExW(g_path, g_old, 1 /* MOVEFILE_REPLACE_EXISTING */);
    h = CreateFileW(g_path, 0x100084UL, 1, 0, 4, 0x80, 0);
  }
  return h;
}

static void write_report(void)
{
  void *h;
  u32 n, done;
  h = open_in(0);      /* the current directory: the game's own files are there */
  if (h == (void *)-1) {
    n = GetModuleFileNameW(0, g_path, 260);
    while (n != 0 && g_path[n - 1] != '\\') n--;
    for (done = 0; done < n; done++) g_old[done] = g_path[done];
    h = n != 0 ? open_in(n) : (void *)-1;
  }
  if (h == (void *)-1) {
    n = GetTempPathW(260, g_path);
    for (done = 0; done < n; done++) g_old[done] = g_path[done];
    h = n != 0 && n < 260 ? open_in(n) : (void *)-1;
  }
  if (h == (void *)-1) return;
  WriteFile(h, g_out, (u32)(g_p - g_out), &done, 0);
  CloseHandle(h);
}

/* one report; at = fault address or the caller of a hooked function; ctx = the CONTEXT of the fault, or g_ctx (eip / esp / ebp only, report_here) */
static void report(const char *reason, u32 code, u32 at, const u32 *ctx)
{
  static const char *const reg[] = { "edi", "esi", "ebx", "edx", "ecx", "eax", "ebp", "eip" };
  struct systime t;
  struct osver v;
  long (__stdcall *get_version)(struct osver *);
  const char *(__cdecl *wine_version)(void);
  const char *cmd;
  char *exe = (char *)GetModuleHandleW(0);
  void *ntdll = GetModuleHandleW(L"ntdll.dll");
  u32 i, sp, w, ebp, found;
  if (_InterlockedExchange(&g_busy, 1) != 0) return;
  g_p = g_out;
  GetSystemTime(&t);
  s("==== MFLng crash report "); dec(t.year); s("-"); dec2(t.month); s("-"); dec2(t.day); s(" "); dec2(t.hour); s(":"); dec2(t.minute); s(":"); dec2(t.second); s(" UTC\r\n");
  s("reason: "); s(reason); s("\r\ncode: "); hex(code); s("   at: "); addr(at);
  s("\r\nexe stamp: "); hex(*(u32 *)(exe + *(u32 *)(exe + 0x3c) + 8)); s(" (\"Timestamp is\" line of the matching .map)   thread: "); dec(GetCurrentThreadId());
  if (g_heap_known) { s("   process heap mode: "); dec(g_heap_mode); s(g_heap_mode == 2 ? " (LFH)" : ""); }
  s("\r\nstate: race_phase="); dec((u32)g_race_phase); s(" menu_current="); dec((u32)g_menu_current); s(" app_state="); dec((u32)g_app_state);
  if (g_fc_count != 0) { s("\r\nlast first-chance fault before: "); hex(g_fc_code); s(" at "); addr(g_fc_addr); s(" (count "); dec(g_fc_count); s(")"); }
  if (ctx != g_ctx) {
    for (i = 0; i < 8; i++) { s(i % 4 == 0 ? "\r\n" : "  "); s(reg[i]); s("="); hex(ctx[C_EDI + i]); }
    s("  esp="); hex(ctx[C_ESP]); s("  efl="); hex(ctx[C_EFL]);
  } else { s("\r\n(not a fault: esp = the hook's return slot) esp="); hex(ctx[C_ESP]); s(" ebp="); hex(ctx[C_EBP]); }
  sp = ctx[C_ESP];
  s("\r\nstack:");
  for (i = 0; i < 32 && stack_ok(sp + 4 * i); i++) { s(i % 8 == 0 ? "\r\n  " : " "); hex(*(u32 *)(sp + 4 * i)); }
  s("\r\nebp chain:");
  for (ebp = ctx[C_EBP], i = 0; i < 24 && stack_ok(ebp) && stack_ok(ebp + 4); i++) {
    s("\r\n  "); addr(*(u32 *)(ebp + 4));
    w = *(u32 *)ebp;
    if (w <= ebp) break;
    ebp = w;
  }
  s("\r\nreturn addresses on the stack:");
  for (found = 0, i = 0; found < 24 && i < 4096 && stack_ok(sp + 4 * i); i++) {
    w = *(u32 *)(sp + 4 * i);
    if (is_return_address(w)) { s("\r\n  [esp+"); hex(4 * i); s("] "); addr(w); found++; }
  }
  cmd = GetCommandLineA();
  s("\r\ncommand line: ");
  for (i = 0; cmd != 0 && cmd[i] != 0 && i < 300; i++) { char c[2] = { cmd[i], 0 }; s(c); }
  get_version = ntdll != 0 ? (long (__stdcall *)(struct osver *))GetProcAddress(ntdll, "RtlGetVersion") : 0;
  v.size = sizeof v;
  if (get_version != 0 && get_version(&v) == 0) { s("\r\nwindows: "); dec(v.major); s("."); dec(v.minor); s("."); dec(v.build); s(" "); ws(v.sp, 40); }
  wine_version = ntdll != 0 ? (const char *(__cdecl *)(void))GetProcAddress(ntdll, "wine_get_version") : 0;
  if (wine_version != 0) { s("  wine "); s(wine_version()); }
  s("\r\n\r\n");
  write_report();
}

/* the CRT / exit paths: no fault context.  esp = the hook's return address slot, so the stack scan starts in the caller's frame; ebp =
   report_here's own frame (inline asm forces one), so the chain runs hook -> the hook's caller.  (RtlCaptureContext assumes a frame pointer
   in its caller, which /O2 code has not.) */
#define REPORT_HERE(reason, code) report_here(reason, code, (u32)_ReturnAddress(), (u32)_AddressOfReturnAddress())
static __declspec(noinline) void report_here(const char *reason, u32 code, u32 caller, u32 sp)
{
  u32 frame;
  __asm mov frame, ebp
  g_ctx[C_EIP] = caller; g_ctx[C_ESP] = sp; g_ctx[C_EBP] = frame;
  report(reason, code, caller, g_ctx);
}

static long __stdcall on_unhandled(struct xptrs *p)
{
  report("unhandled exception", p->rec->code, (u32)p->rec->addr, p->ctx);
  return g_prev_filter != 0 ? g_prev_filter(p) : 0 /* EXCEPTION_CONTINUE_SEARCH: Windows ends the process as without us */;
}

static long __stdcall on_first_chance(struct xptrs *p)
{
  u32 code = p->rec->code;
  /* stack overflow, stack buffer overrun, heap corruption, fatal app exit: ntdll raises heap corruption (RtlReportCriticalFailure) inside its
     own handler, which hands it to WER directly: the unhandled filter never sees it, this handler does; the EBP chain from ntdll's frames
     leads to the game's free / malloc caller */
  if (code == 0xc00000fdUL || code == 0xc0000409UL || code == 0xc0000374UL || code == 0x40000015UL)
    report("fatal exception (first chance)", code, (u32)p->rec->addr, p->ctx);
  else if (code == 0xc0000005UL || code == 0xc000001dUL || code == 0xc0000096UL || code == 0xc0000094UL) {      /* AV, illegal / privileged instruction, int divide */
    g_fc_code = code; g_fc_addr = (u32)p->rec->addr; g_fc_count++;      /* may be handled; reported with the next abnormal end */
  }
  return 0;      /* EXCEPTION_CONTINUE_SEARCH */
}

static void __cdecl on_abort(int sig) { REPORT_HERE("abort() (SIGABRT)", (u32)sig); }
static void __cdecl on_purecall(void) { REPORT_HERE("pure virtual function call", 0); }      /* the CRT then calls abort() */
static void __cdecl on_invalid_parameter(const wchar_t *expr, const wchar_t *fn, const wchar_t *file, unsigned int line, uintptr_t reserved)
{
  (void)expr; (void)fn; (void)file; (void)line; (void)reserved;
  REPORT_HERE("CRT invalid parameter", 0);
  __fastfail(5);      /* FAST_FAIL_INVALID_ARG: what the release UCRT does without a handler */
}
static void __cdecl hook_exit(int code) { if (code != 0) REPORT_HERE("exit() with a non-zero code", (u32)code); g_exit(code); }
static void __cdecl hook_exit2(int code) { if (code != 0) REPORT_HERE("_exit() with a non-zero code", (u32)code); g_exit2(code); }
static void __stdcall hook_exitprocess(u32 code) { if (code != 0) REPORT_HERE("ExitProcess() with a non-zero code", code); g_exitprocess(code); }
static int __stdcall hook_terminate(void *process, u32 code)
{
  if (process == (void *)-1 && code != 0) REPORT_HERE("TerminateProcess(self) with a non-zero code", code);
  return g_terminate(process, code);
}

/* every slot of our import table holding `real` -> hook; returns real (0 = not imported / not found) */
static void *hook_iat(void *module, const char *name, void *hook)
{
  char *exe = (char *)GetModuleHandleW(0);
  u32 *desc, *slot, old;
  void *real = module != 0 ? GetProcAddress(module, name) : 0;
  u32 rva = *(u32 *)(exe + *(u32 *)(exe + 0x3c) + 0x80);      /* DataDirectory[IMPORT] */
  if (real == 0 || rva == 0) return 0;
  for (desc = (u32 *)(exe + rva); desc[3] != 0; desc += 5)      /* IMAGE_IMPORT_DESCRIPTOR: Name +12, FirstThunk +16 */
    for (slot = (u32 *)(exe + desc[4]); *slot != 0; slot++)
      if (*slot == (u32)real && VirtualProtect(slot, 4, 0x04 /* PAGE_READWRITE */, &old)) { *slot = (u32)hook; VirtualProtect(slot, 4, old, &old); }
  return real;
}

static void __cdecl crashlog_install(void)
{
  void *kernel_dll = GetModuleHandleW(L"kernel32.dll"), *ucrt = GetModuleHandleW(L"ucrtbase.dll");
  u32 guarantee = 16384;      /* stack left for the report after a stack overflow (main thread) */
  SetThreadStackGuarantee(&guarantee);
  g_heap_known = HeapQueryInformation(GetProcessHeap(),0 /* HeapCompatibilityInformation */, &g_heap_mode, sizeof g_heap_mode, 0);      /* at start: a corrupt heap is not read later */
  g_prev_filter = SetUnhandledExceptionFilter(on_unhandled);
  AddVectoredExceptionHandler(1, on_first_chance);
  _set_invalid_parameter_handler(on_invalid_parameter);
  _set_purecall_handler(on_purecall);
  signal(SIGABRT, on_abort);
  g_exit = (void (__cdecl *)(int))hook_iat(ucrt, "exit", (void *)hook_exit);      /* /MD: the CRT start-up's exit(WinMain result) comes through this slot */
  g_exit2 = (void (__cdecl *)(int))hook_iat(ucrt, "_exit", (void *)hook_exit2);
  g_exitprocess = (void (__stdcall *)(u32))hook_iat(kernel_dll, "ExitProcess", (void *)hook_exitprocess);
  g_terminate = (int (__stdcall *)(void *, u32))hook_iat(kernel_dll, "TerminateProcess", (void *)hook_terminate);
}

/* Installed by the CRT start-up right after its own pre_cpp_initialization (.CRT$XCAA, which sets the CRT's unhandled filter without
   chaining: an earlier install would be overwritten; ours chains to it, so a C++ exception still ends in terminate()), before the game's
   .CRT$XCU initialisers and before WinMain. */
#pragma section(".CRT$XCAB", read)
__declspec(allocate(".CRT$XCAB")) void (__cdecl *const mfl_crashlog_entry)(void) = crashlog_install;
#endif
