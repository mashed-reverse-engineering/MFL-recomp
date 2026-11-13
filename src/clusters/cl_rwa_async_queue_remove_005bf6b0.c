// cluster-entries: rwa_async_queue_remove_005bf6b0
/* Installed cluster rwa_async_queue_remove_005bf6b0 (tools/cluster_install.py from output/cluster_pilot_auw7/rwa_async_queue_remove_005bf6b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) asynchronous file i/o: MFL.exe 0x005bf6b0 takes a request out of a queue (register
   function: queue in esi, request in edi).  Written from the MFL.exe disassembly (objdump). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaAsyncLink { struct RwaAsyncLink *next, *prev; } RwaAsyncLink;
typedef struct RwaAsyncQueue {
  RwaAsyncLink head;     /* +0x00 */
  void *lock;            /* +0x08 */
  RwUInt32 count;        /* +0x0c */
  void *wake;            /* +0x10 */
  void *idle;            /* +0x14 */
} RwaAsyncQueue;
#define REQUEST_LINK 0x30      /* the queue link's offset in a request */

/* Unlinks every queue entry of req (the scan goes on to the end).  Returns the ReleaseSemaphore result. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwBool rwa_async_queue_remove_005bf6b0_n(RwaAsyncQueue *q, void *req)
{
  RwaAsyncLink *l;

  WaitForSingleObject(q->lock, 0xffffffff);
  for (l = q->head.next; l != &q->head; l = l->next)
    if ((char *)l - REQUEST_LINK == (char *)req) {
      l->prev->next = l->next;
      l->next->prev = l->prev;
      q->count--;
    }
  return ReleaseSemaphore(q->lock, 1, 0);
}

/* entry thunk at the original address of rwa_async_queue_remove_005bf6b0: register inputs (esi,edi) -> natural C rwa_async_queue_remove_005bf6b0_n; ret 0 */
__declspec(naked) void rwa_async_queue_remove_005bf6b0(void)
{
    __asm {
        push edi
        push esi
        call rwa_async_queue_remove_005bf6b0_n
        add esp, 8
        ret
    }
}
