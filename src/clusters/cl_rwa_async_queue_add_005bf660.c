// cluster-entries: rwa_async_queue_add_005bf660 rwa_async_file_open_005bf6f0
/* Installed cluster rwa_async_queue_add_005bf660 (tools/cluster_install.py from output/cluster_pilot_auw7/rwa_async_queue_add_005bf660.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) asynchronous file i/o: the request queue of the i/o thread.  MFL.exe 0x005bf660 adds a
   request (register function: request in eax, queue in esi) and is called by the async open 0x005bf6f0.  Written from the MFL.exe
   disassembly (objdump). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef struct RwaAsyncLink { struct RwaAsyncLink *next, *prev; } RwaAsyncLink;
typedef struct RwaAsyncQueue {
  RwaAsyncLink head;     /* +0x00 */
  void *lock;            /* +0x08 semaphore guarding the queue */
  RwUInt32 count;        /* +0x0c requests queued */
  void *wake;            /* +0x10 signalled when the queue becomes non-empty */
  void *idle;            /* +0x14 */
} RwaAsyncQueue;

typedef RwInt32 (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);
typedef struct RwaAsyncRequest {      /* 0x38 bytes, from the free list 0x7dbc74 */
  void *file;            /* +0x00 */
  void *fp;              /* +0x04 */
  RwUInt32 w8;
  RwUInt32 op;           /* +0x0c: 1 open, 3 seek, 4 read */
  const char *name;      /* +0x10 */
  void *buffer;          /* +0x14 */
  RwUInt32 size;         /* +0x18 */
  RwUInt32 w1c;
  RwaAsyncCallback callback;      /* +0x20 */
  void *data;            /* +0x24 */
  void *done;            /* +0x28 semaphore, signalled when the request is finished */
  RwUInt32 w2c;
  RwaAsyncLink link;     /* +0x30 in the queue */
} RwaAsyncRequest;

#define ASYNC_OPEN 1
#define RESULT_FAILED 2

/*007dbc74*/ extern undefined g_data_7dbc74;          /* free list of requests */
/*007dbc48*/ extern RwaAsyncQueue g_data_7dbc48;      /* the i/o thread's queue */
/*005bec50*/ extern void *rwa_unit243670_005bec50(void *freeList, RwUInt32 hint);      /* RwaFreeListAlloc */
/*005bed80*/ extern void RwaFreeListFree(void *freeList, void *block);
/*005c4340*/ extern RwBool RwaSemaphoreCreate(void **semaphore, RwInt32 initial, RwInt32 maximum);

/* Adds req at the front of q and wakes the thread when it was the only one.  Returns the last ReleaseSemaphore result. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwBool rwa_async_queue_add_005bf660_n(RwaAsyncRequest *req, RwaAsyncQueue *q)
{
  RwBool r;

  WaitForSingleObject(q->lock, 0xffffffff);
  req->link.next = q->head.next;
  req->link.prev = &q->head;
  q->head.next->prev = &req->link;
  q->head.next = &req->link;
  q->count++;
  r = ReleaseSemaphore(q->lock, 1, 0);
  if (q->count == 1)
    r = ReleaseSemaphore(q->wake, 1, 0);
  return r;
}

/* Queues an open of `name` for the i/o thread; callback(request, 0, result, ASYNC_OPEN... ) reports it.  When no request or
   semaphore can be had the callback runs at once with result 2 (failed) and the request (0 or the freed block). */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwInt32 rwa_async_file_open_005bf6f0(const char *name, const char *mode, RwaAsyncCallback callback, void *data)
{
  RwaAsyncRequest *req = rwa_unit243670_005bec50(&g_data_7dbc74, 0x30801);

  if (!req)
    return callback(0, 0, RESULT_FAILED, 2, data);
  req->file = req;
  req->fp = 0;
  req->op = 0;
  req->name = 0;
  req->w1c = 0;
  req->size = 0;
  req->buffer = 0;
  req->callback = 0;
  req->data = 0;
  req->w2c = 0;
  req->link.prev = 0;
  req->link.next = 0;
  req->w8 = 0;
  if (!RwaSemaphoreCreate(&req->done, 0, 1)) {
    RwaFreeListFree(&g_data_7dbc74, req);
    return callback(req, 0, RESULT_FAILED, 2, data);
  }
  req->callback = callback;
  req->data = data;
  req->name = name;
  req->op = ASYNC_OPEN;
  return rwa_async_queue_add_005bf660_n(req, &g_data_7dbc48);
}

/* entry thunk at the original address of rwa_async_queue_add_005bf660: register inputs (eax,esi) -> natural C rwa_async_queue_add_005bf660_n; ret 0 */
__declspec(naked) void rwa_async_queue_add_005bf660(void)
{
    __asm {
        push esi
        push eax
        call rwa_async_queue_add_005bf660_n
        add esp, 8
        ret
    }
}
