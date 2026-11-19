/* v143md_shim.c: CC=v143 LIB_CRT=v143md only (W16 V143 item 2; tools/delink/link_ours.sh compiles it with v143 next to the generated
   crt_xcu.c).  The VC7 CRT names of the game that the v143 /MD CRT has no counterpart for.  (_ftol needs no shim: ucrt.lib exports it.) */
typedef struct _iobuf FILE;      /* opaque here (mfl_tier2.h spelling) */
extern FILE *__cdecl __acrt_iob_func(unsigned int index);
extern unsigned int __cdecl fwrite(const void *p, unsigned int size, unsigned int n, FILE *fp);
extern int __stdcall WinMain(void *inst, void *prev, char *cmdline, int show);

/* msvcrt.lib's WinMainCRTStartup calls _WinMain@16; this reference makes mkobj define it at the game's WinMain (recomp/aliases.txt) */
void *const mfl_v143_winmain = (void *)WinMain;

/* recomp/automatch_near/an_rwp_qhull_glue.c prints qhull's stdout / stderr text with _fwrite(buf, 1, n, &_iob[1 or 2]) (VC7 CRT FILE
   array at 0x62c690).  The UCRT has no _iob: this dummy array takes the name, and _fwrite maps &_iob[k] to __acrt_iob_func(k); any other
   FILE * goes to fwrite as it is. */
typedef struct { char raw[0x20]; } mfl_iobuf;
mfl_iobuf _iob[3];

unsigned int __cdecl _fwrite(const void *p, unsigned int size, unsigned int n, FILE *fp)
{
  unsigned int k = (unsigned int)((char *)fp - (char *)_iob) / sizeof(mfl_iobuf);

  return fwrite(p, size, n, k < 3 ? __acrt_iob_func(k) : fp);
}

/* msvcrt.lib(utility.obj) start-up calls.  With /MD the vcruntime140 / ucrtbase DLLs initialise and tear down themselves (their own
   DllMain), so the exe's start-up only gets stubs: msvcrt.lib(ucrt_stubs.obj) __scrt_stub_for_acrt_* return true and
   __scrt_stub_for_is_c_termination_complete returns 0; utility.obj maps the 11 names to them with /alternatename (the ___vcrt_* ones to
   the acrt stubs), which this link does not resolve (V143c5: /include of the stubs loaded them, still LNK2019).  Same bodies here.
   bool = unsigned char (cl x86 returns it in al). */
typedef unsigned char mfl_bool;
mfl_bool __cdecl __vcrt_initialize(void) { return 1; }
mfl_bool __cdecl __vcrt_uninitialize(mfl_bool terminating) { (void)terminating; return 1; }
mfl_bool __cdecl __vcrt_uninitialize_critical(mfl_bool terminating) { (void)terminating; return 1; }
mfl_bool __cdecl __vcrt_thread_attach(void) { return 1; }
mfl_bool __cdecl __vcrt_thread_detach(void) { return 1; }
mfl_bool __cdecl __acrt_initialize(void) { return 1; }
mfl_bool __cdecl __acrt_uninitialize(mfl_bool terminating) { (void)terminating; return 1; }
mfl_bool __cdecl __acrt_uninitialize_critical(mfl_bool terminating) { (void)terminating; return 1; }
mfl_bool __cdecl __acrt_thread_attach(void) { return 1; }
mfl_bool __cdecl __acrt_thread_detach(void) { return 1; }
int __cdecl _is_c_termination_complete(void) { return 0; }

/* V143d1: the image's library code (RenderWare, D3DX, zlib/png, piz file I/O) still calls the VC7 CRT bodies section relative, and those
   use VC7 state that only the VC7 WinMainCRTStartup set up (heap, TLS + locks, lowio, stdio, fp conversion, the __xi table): left zero,
   the first VC7 malloc / fopen / per-thread call faults (v143i: page fault in ntdll under Wine).  This .CRT$XIC entry repeats that start-up
   (crt0.c order) before msvcrt.lib's _initterm reaches .CRT$XCU.  The VC7 heap is the PROCESS heap (what _heap_init picks on NT 5+, minus
   its HeapCreate): ucrt allocates there too, so a block crossing the two CRTs (malloc in one, free in the other) stays valid. */
__declspec(dllimport) void *__stdcall GetProcessHeap(void);
__declspec(dllimport) unsigned long __stdcall GetVersion(void);
extern void *_crtheap;
extern int __active_heap, _osplatform, _osver, _winver, _winmajor, _winminor;
extern int __cdecl _mtinit(void);
extern void __cdecl _ioinit(void);      /* VC7.0 returns nothing, VC7.1 -1 on failure: not read */
extern void __cdecl _fpmath(int init_precision);
typedef int (__cdecl *mfl_pifv)(void);
extern const mfl_pifv g_data_5fb03c[], g_data_5fb058[];      /* the image's VC7 __xi_a .. __xi_z (onexitinit, initstdio, mbctable ...) */

static int __cdecl mfl_vc7_crt_init(void)
{
  unsigned long v = GetVersion();
  const mfl_pifv *p;
  int r;

  _osplatform = v & 0x80000000u ? 1 : 2;
  _winmajor = v & 0xff;
  _winminor = (v >> 8) & 0xff;
  _winver = (_winmajor << 8) + _winminor;
  _osver = ((v >> 16) & 0x7fff) | (v & 0x80000000u ? 0x8000 : 0);
  _crtheap = GetProcessHeap();
  __active_heap = 1;      /* __SYSTEM_HEAP */
  if (!_mtinit()) return 16;      /* _RT_THREAD */
  _ioinit();
  _fpmath(1);
  for (p = g_data_5fb03c; p < g_data_5fb058; ++p)
    if (*p && (r = (*p)()) != 0) return r;
  return 0;
}

#pragma section(".CRT$XIC", read)
__declspec(allocate(".CRT$XIC")) const mfl_pifv mfl_vc7_crt_init_entry = mfl_vc7_crt_init;
