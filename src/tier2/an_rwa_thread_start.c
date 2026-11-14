/* RenderWare Audio (Criterion Software) worker thread start, MFL.exe 0x005c4400 (_RwaThreadStart) and its thread procedure
 * 0x005c43d0.  Written from the MFL.exe disassembly (capstone).  Thread side effects: tools/shadow_exclude.txt lines. */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) void __stdcall Sleep(int milliseconds);
extern __declspec(dllimport) RwBool __stdcall SetThreadPriority(void *thread, int priority);
/*005d4159*/ extern RwUInt32 __cdecl _beginthread(void (__cdecl *start)(void *), RwUInt32 stack_size, void *arg);

typedef struct RwaThread {
  RwUInt32 handle;                 /* +0x00 _beginthread's result */
  void **(*run)(void *arg);        /* +0x04 the work function; returns the semaphore holder to signal, or NULL */
  void *arg;                       /* +0x08 */
  int priority;                    /* +0x0c */
  RwUInt32 stack_size;             /* +0x10 */
} RwaThread;

/* thread procedure: runs the work function, signals its semaphore and then sleeps for ever (the thread is never joined) */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void __cdecl gap_5c43d0(RwaThread *thread)
{
  void **done = thread->run(thread->arg);

  if (done) {
    ReleaseSemaphore(*done, 1, 0);
    Sleep(-1);
  }
}

/* starts the thread with arg; the thread object, or NULL when _beginthread failed */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwaThread *_RwaThreadStart(RwaThread *thread, void *arg)
{
  RwUInt32 handle;

  thread->arg = arg;
  handle = _beginthread((void (__cdecl *)(void *))gap_5c43d0, thread->stack_size, thread);
  if (handle == (RwUInt32)-1)
    return 0;
  thread->handle = handle;
  SetThreadPriority((void *)handle, thread->priority);
  return thread;
}
