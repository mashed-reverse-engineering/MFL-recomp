// cluster-entries: rwa_async_queue_init_005bf5e0
/* Installed cluster rwa_async_queue_init_005bf5e0 (tools/cluster_install.py from output/cluster_pilot_auw7/rwa_async_queue_init_005bf5e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) asynchronous file i/o: MFL.exe 0x005bf5e0 sets up a request queue (register function:
   queue in esi) and the module's global semaphore 0x7dbc44.  Written from the MFL.exe disassembly (objdump). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef struct RwaAsyncLink { struct RwaAsyncLink *next, *prev; } RwaAsyncLink;
typedef struct RwaAsyncQueue {
  RwaAsyncLink head;     /* +0x00 */
  void *lock;            /* +0x08 */
  RwUInt32 count;        /* +0x0c */
  void *wake;            /* +0x10 */
  void *idle;            /* +0x14 */
} RwaAsyncQueue;

/*007dbc44*/ extern void *g_data_7dbc44;
/*005c4340*/ extern RwBool RwaSemaphoreCreate(void **semaphore, RwInt32 initial, RwInt32 maximum);

/* An empty queue with its semaphores.  Returns q, or 0 when a semaphore could not be created (the ones made so far are closed,
   except `wake`: the original leaves that one open). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwaAsyncQueue *rwa_async_queue_init_005bf5e0_n(RwaAsyncQueue *q)
{
  q->count = 0;
  q->head.next = &q->head;
  q->head.prev = &q->head;
  if (!RwaSemaphoreCreate(&q->lock, 1, 1))
    return 0;
  if (RwaSemaphoreCreate(&q->wake, 0, 1) && RwaSemaphoreCreate(&q->idle, 1, 1)) {
    if (RwaSemaphoreCreate(&g_data_7dbc44, 1, 1))
      return q;
    CloseHandle(q->idle);
  }
  CloseHandle(q->lock);
  return 0;
}

/* entry thunk at the original address of rwa_async_queue_init_005bf5e0: register inputs (esi) -> natural C rwa_async_queue_init_005bf5e0_n; ret 0 */
__declspec(naked) void rwa_async_queue_init_005bf5e0(void)
{
    __asm {
        push esi
        call rwa_async_queue_init_005bf5e0_n
        add esp, 4
        ret
    }
}
