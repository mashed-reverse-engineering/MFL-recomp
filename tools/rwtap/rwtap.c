/* rwtap: logs calls into the RenderWare 3.7 layer of the (original or standalone) MFL.exe.  Loaded by the d3dtrace proxy (cfg `rwtap=<funcs.txt>`),
 * hooks every function of funcs.txt by an inline detour (displaced prologue + jmp back), logs the 8 stack dwords at entry (cdecl args, hex) and the
 * pointed-to data named by the spec.  Return values are not captured.  Format: see README.md. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
static FILE *lg; static CRITICAL_SECTION cs; static volatile int *p_active, *p_ndraws, *p_nframes;
static unsigned seq, ncall; static char names[128][48]; static char specs[128][96]; static unsigned nargs[128];
static void dump(const u8 *p, unsigned n) {
    if (n > 65536) n = 65536;
    if (IsBadReadPtr(p, n)) { fputs("!", lg); return; }
    for (unsigned i = 0; i < n; i++) fprintf(lg, "%02x", p[i]);
}
/* called by the stub with the idx and a pointer at the return address; args follow it */
static void __cdecl on_call(unsigned idx, const unsigned *ra) {
    if (!*p_active) return;
    const unsigned *a = ra + 1; EnterCriticalSection(&cs);
    fprintf(lg, "C %u %s d=%d f=%d a=", seq++, names[idx], *p_ndraws, *p_nframes);
    for (unsigned i = 0; i < nargs[idx]; i++) fprintf(lg, i ? ",%x" : "%x", a[i]);
    char sp[96]; strcpy(sp, specs[idx]);
    for (char *t = strtok(sp, " "); t; t = strtok(NULL, " ")) {
        int k = atoi(t + 1); const char *c = strchr(t, ':') + 1; unsigned n;
        if (*c == 'n') { int m = atoi(c + 1); n = a[m] * (unsigned)atoi(strchr(c, '*') + 1); } else n = (unsigned)atoi(c);
        fprintf(lg, " p%d=", k); dump((const u8 *)(uintptr_t)a[k], n);
    }
    fputc('\n', lg); LeaveCriticalSection(&cs);
}
static int hook(unsigned idx, u8 *fn, unsigned len) {
    u8 *s = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); if (!s) return 0; u8 *q = s;
    *q++ = 0x9c; *q++ = 0x60;                       /* pushfd; pushad */
    *q++ = 0x8d; *q++ = 0x44; *q++ = 0x24; *q++ = 0x24;   /* lea eax,[esp+36] = return address of the hooked call */
    *q++ = 0x50; *q++ = 0x68; memcpy(q, &idx, 4); q += 4;  /* push eax; push idx */
    *q++ = 0xb8; void *f = (void *)on_call; memcpy(q, &f, 4); q += 4; *q++ = 0xff; *q++ = 0xd0;   /* mov eax,on_call; call eax */
    *q++ = 0x83; *q++ = 0xc4; *q++ = 0x08;          /* add esp,8 */
    *q++ = 0x61; *q++ = 0x9d;                       /* popad; popfd */
    memcpy(q, fn, len); q += len;                   /* displaced instructions */
    *q++ = 0xe9; int rel = (int)((u8 *)fn + len - (q + 4)); memcpy(q, &rel, 4); q += 4;
    DWORD old; if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &old)) return 0;
    fn[0] = 0xe9; rel = (int)(s - (fn + 5)); memcpy(fn + 1, &rel, 4); for (unsigned i = 5; i < len; i++) fn[i] = 0x90;
    VirtualProtect(fn, len, old, &old); FlushInstructionCache(GetCurrentProcess(), fn, len); return 1;
}
/* funcs: funcs.txt path; delta: hex offset added to every VA (standalone link); returns number of hooks */
__declspec(dllexport) int rwtap_init(const char *funcs, const char *out, unsigned delta, volatile int *active, volatile int *ndraws, volatile int *nframes) {
    InitializeCriticalSection(&cs); p_active = active; p_ndraws = ndraws; p_nframes = nframes;
    lg = fopen(out, "wb"); FILE *f = fopen(funcs, "r"); if (!lg || !f) return -1;
    setvbuf(lg, NULL, _IOFBF, 1 << 20); fprintf(lg, "RWTAP 1 delta=%x\n", delta);
    char ln[256]; int n = 0;
    while (fgets(ln, sizeof ln, f) && ncall < 128) {
        if (ln[0] == '#') continue; char nm[64], sp[200] = ""; unsigned va, len, na_;
        char *e = ln + strcspn(ln, "\r\n"); *e = 0;
        int k = sscanf(ln, "%47s %x %u %u %95[^\n]", nm, &va, &len, &na_, sp); if (k < 4) continue; nargs[ncall] = na_ > 8 ? 8 : na_;
        strcpy(names[ncall], nm); strcpy(specs[ncall], k > 4 ? sp : "");
        if (hook(ncall, (u8 *)(uintptr_t)(va + delta), len)) { fprintf(lg, "H %u %s %x\n", ncall, nm, va + delta); n++; }
        ncall++;
    }
    fclose(f); fflush(lg); return n;
}
BOOL WINAPI DllMain(HINSTANCE h, DWORD r, void *x) { return TRUE; }
