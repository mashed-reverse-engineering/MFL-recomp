/* crt_raise = raise (0x004c286b, libcmt winsig.obj; the member cannot be linked: it imports SetConsoleCtrlHandler, which MFL.exe lacks).
   The only caller is abort (0x4c13d3, raise(SIGABRT)).  siglookup (0x4c283d) is its static helper; in libcmt it takes signum / table in
   esi / edx (link-time code generation), here a plain static function of this file (its 46 original bytes have no other caller). */
#include "mfl_tier2.h"

#define SIGINT 2
#define SIGILL 4
#define SIGFPE 8
#define SIGSEGV 11
#define SIGTERM 15
#define SIGBREAK 21
#define SIGABRT 22
#define SIG_DFL ((MflSigHandler)0)
#define SIG_IGN ((MflSigHandler)1)
#define FPE_EXPLICITGEN 0x8c   /* _FPE_EXPLICITGEN: raise(SIGFPE) called by the program */
#define SIGNAL_LOCK 0   /* _SIGNAL_LOCK */
#define EXIT_ABORT 3

typedef void (__cdecl *MflSigHandler)(int);
typedef void (__cdecl *MflFpeHandler)(int, int);

/* one row of the per-thread exception-action table (_XCPT_ACTION): exception code -> signal and its handler */
typedef struct MflXcptAction {
  unsigned int xcptnum;
  int signum;   /* 0x4 */
  MflSigHandler action;   /* 0x8 */
} MflXcptAction;

/* the part of the CRT per-thread data (_tiddata) that raise uses */
typedef struct MflTidData {
  unsigned char pad[0x54];
  MflXcptAction *xcptacttab;   /* 0x54: _pxcptacttab, this thread's copy of the action table */
  void *xcptinfoptrs;   /* 0x58: _tpxcptinfoptrs, EXCEPTION_POINTERS of the exception being handled */
  int fpecode;   /* 0x5c: _tfpecode */
} MflTidData;

/*0062d114*/ extern int _XcptActTabCount;
/*0062d108*/ extern int _First_FPE_Indx;
/*0062d10c*/ extern int _Num_FPE;
/*00771f4c*/ extern MflSigHandler ctrlc_action;   /* SIGINT */
/*00771f50*/ extern MflSigHandler ctrlbreak_action;   /* SIGBREAK */
/*00771f54*/ extern MflSigHandler abort_action;   /* SIGABRT */
/*00771f58*/ extern MflSigHandler term_action;   /* SIGTERM */
extern MflTidData *__cdecl _getptd(void);
extern void __cdecl _exit(int code);
extern void __cdecl _lock(int locknum);
extern void __cdecl _unlock(int locknum);

/* libcmt's winsig.obj is size optimised (raise builds its __try frame with __SEH_prolog / __SEH_epilog): the whole file is too */
#pragma optimize("s", on)
/* The table row of signum, or NULL.  Like the original, the first row is compared before the bound is checked. */
static MflXcptAction *crt_signal_find_action(int signum, MflXcptAction *tab)
{
  MflXcptAction *p = tab;
  int n = _XcptActTabCount;

  do {
    if (p->signum == signum) break;
    p++;
  } while (p < tab + n);
  if (p < tab + n && p->signum == signum) return p;
  return 0;
}

/* Sends signum to the program: SIG_IGN -> nothing, SIG_DFL -> _exit(3), else the handler is reset to SIG_DFL (for SIGFPE: every FPE row
   of the thread's table) and called; SIGFPE handlers get the FPE code (FPE_EXPLICITGEN) as a second argument.  SIGINT / SIGBREAK /
   SIGABRT / SIGTERM handlers are global (reset under the signal lock), SIGFPE / SIGILL / SIGSEGV ones per thread (the exception
   pointers are hidden from the handler while it runs).  Returns 0, or -1 for an unknown signal. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int crt_raise(int signum)
{
  MflSigHandler action, *slot;
  MflTidData *ptd;
  int global = 0, i;
  void *old_infoptrs;
  int old_fpecode;

  switch (signum) {
  case SIGINT: slot = &ctrlc_action; action = *slot; global = 1; break;
  case SIGBREAK: slot = &ctrlbreak_action; action = *slot; global = 1; break;
  case SIGABRT: slot = &abort_action; action = *slot; global = 1; break;
  case SIGTERM: slot = &term_action; action = *slot; global = 1; break;
  case SIGILL:
  case SIGFPE:
  case SIGSEGV:
    ptd = _getptd();
    slot = &crt_signal_find_action(signum, ptd->xcptacttab)->action;
    action = *slot;
    break;
  default:
    return -1;
  }
  if (action == SIG_IGN) return 0;
  if (action == SIG_DFL) _exit(EXIT_ABORT);
  if (global) _lock(SIGNAL_LOCK);
  __try {
    if (signum == SIGFPE || signum == SIGSEGV || signum == SIGILL) {
      old_infoptrs = ptd->xcptinfoptrs;
      ptd->xcptinfoptrs = 0;
      if (signum == SIGFPE) {
        old_fpecode = ptd->fpecode;
        ptd->fpecode = FPE_EXPLICITGEN;
      }
    }
    if (signum == SIGFPE) {
      for (i = _First_FPE_Indx; i < _First_FPE_Indx + _Num_FPE; i++)
        ptd->xcptacttab[i].action = SIG_DFL;
    } else {
      *slot = SIG_DFL;
    }
  } __finally {
    if (global) _unlock(SIGNAL_LOCK);
  }
  if (signum == SIGFPE)
    ((MflFpeHandler)action)(SIGFPE, ptd->fpecode);
  else
    action(signum);
  if (signum == SIGFPE || signum == SIGSEGV || signum == SIGILL) {
    ptd->xcptinfoptrs = old_infoptrs;
    if (signum == SIGFPE) ptd->fpecode = old_fpecode;
  }
  return 0;
}
