// cluster-entries: rwa_async_queue_destroy_005bfba0 rwa_async_module_open_005bfc50 rwa_async_module_close_005bfcf0 rwaFileClose_005bfbf0 rwa_async_file_cancel_005bf8f0
/* Installed cluster rwa_async_queue_destroy_005bfba0 (tools/cluster_install.py from output/cluster_pilot_auw8/rwa_async_queue_destroy_005bfba0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) asynchronous file i/o module: MFL.exe 0x005bfba0 destroys a request queue (register
   function: queue in esi), 0x005bfc50 / 0x005bfcf0 open / close the module (free list, queue, i/o thread 0x005bf970), 0x005bfbf0
   closes an async file and 0x005bf8f0 cancels its queued requests.  Written from the MFL.exe disassembly (capstone). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef struct RwaAsyncLink { struct RwaAsyncLink *next, *prev; } RwaAsyncLink;
typedef struct RwaAsyncQueue {
  RwaAsyncLink head;     /* +0x00 */
  void *lock;            /* +0x08 semaphore guarding the queue */
  RwUInt32 count;        /* +0x0c requests queued */
  void *wake;            /* +0x10 signalled when the queue becomes non-empty */
  void *idle;            /* +0x14 signalled after every request */
} RwaAsyncQueue;

typedef struct RwaAsyncFile {      /* a request block (0x38 bytes) that stands for an open file */
  void *self;            /* +0x00 */
  void *fp;              /* +0x04 */
  char pad08[0x20];
  void *done;            /* +0x28 semaphore */
} RwaAsyncFile;

/*005bed80*/ extern RwInt32 RwaFreeListFree(void *list, void *block);
/*005beec0*/ extern RwInt32 RwaFreeListDestroy(void *list);
/*005bef40*/ extern void *RwaFreeListCreate(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwUInt32 flags, void *list, RwUInt32 hint);
/*005c43a0*/ extern void *_RwaThreadCreate(void *thread, void *(*run)(void *arg), RwUInt32 priority, RwUInt32 stack_size);
/*005c4400*/ extern void *_RwaThreadStart(void *thread, void *arg);
/* (lane AU W8: the two register callees are the W7 clusters rwa_async_queue_init_005bf5e0 / rwa_async_queue_remove_005bf6b0; the
   pilot file and the harness use their symbols.txt names (Ghidra's default names; the `=@` option is keyed by that name);
   this installed copy calls the same addresses by the alias names, so the object is the same up to the symbol names) */
/*005bf970*/ extern void *rwa_async_io_thread_005bf970(void *arg);
/*00560e70*/ extern RwInt32 rwa_file_close_00560e70(void *fp);

/*007dbc44*/ extern void *g_data_7dbc44;            /* callback lock */
/*007dbc48*/ extern RwaAsyncQueue g_data_7dbc48;    /* the i/o thread's queue */
/*007dbc54*/ extern RwUInt32 g_data_7dbc54;         /* = g_data_7dbc48.count */
/*007dbc58*/ extern void *g_data_7dbc58;            /* = g_data_7dbc48.wake */
/*007dbc5c*/ extern void *g_data_7dbc5c;            /* = g_data_7dbc48.idle */
/*007dbc50*/ extern void *g_data_7dbc50;            /* = g_data_7dbc48.lock */
/*007dbc60*/ extern undefined g_data_7dbc60;        /* the thread object */
/*007dbc74*/ extern undefined g_data_7dbc74;        /* free list of requests */
/*007dbc98*/ extern RwUInt32 g_data_7dbc98;         /* the thread runs */
/*00649610*/ extern RwUInt32 g_data_649610;         /* thread priority */

void rwa_async_queue_destroy_005bfba0_n(RwaAsyncQueue *q);

static RwBool rwa_async_queue_init_005bf5e0_r(RwaAsyncQueue * queue);
extern void rwa_async_queue_init_005bf5e0(void);
static RwBool rwa_async_queue_remove_005bf6b0_r(RwaAsyncQueue * queue, void * request);
extern void rwa_async_queue_remove_005bf6b0(void);
/* waits until the queue is empty, then closes its three semaphores and the callback lock */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rwa_async_queue_destroy_005bfba0_n(RwaAsyncQueue *q)
{
  while (q->count)
    WaitForSingleObject(q->idle, 0xffffffff);
  CloseHandle(q->lock);
  CloseHandle(q->wake);
  CloseHandle(q->idle);
  CloseHandle(g_data_7dbc44);
}

/* module open: request free list, queue, i/o thread.  1: running */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool rwa_async_module_open_005bfc50(void)
{
  g_data_7dbc98 = 1;
  if (RwaFreeListCreate(0x38, 8, 4, 0, &g_data_7dbc74, 0x40801)) {
    if (rwa_async_queue_init_005bf5e0_r(&g_data_7dbc48)) {
      if (_RwaThreadCreate(&g_data_7dbc60, rwa_async_io_thread_005bf970, g_data_649610, 0x1000) && _RwaThreadStart(&g_data_7dbc60, 0))
        return 1;
      rwa_async_queue_destroy_005bfba0_n(&g_data_7dbc48);
    }
    RwaFreeListDestroy(&g_data_7dbc74);
  }
  g_data_7dbc98 = 0;
  return 0;
}

/* module close: waits for the queued requests, stops the thread (flag + wake) and waits for its last signal, frees everything */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void rwa_async_module_close_005bfcf0(void)
{
  while (g_data_7dbc54)
    WaitForSingleObject(g_data_7dbc5c, 0xffffffff);
  g_data_7dbc98 = 0;
  ReleaseSemaphore(g_data_7dbc58, 1, 0);
  WaitForSingleObject(g_data_7dbc50, 0xffffffff);
  if (g_data_7dbc54 == 0) {
    ReleaseSemaphore(g_data_7dbc50, 1, 0);
    WaitForSingleObject(g_data_7dbc58, 0xffffffff);
  } else
    ReleaseSemaphore(g_data_7dbc50, 1, 0);
  rwa_async_queue_destroy_005bfba0_n(&g_data_7dbc48);
  RwaFreeListDestroy(&g_data_7dbc74);
}

/* closes an async file: drops its queued requests, closes the stream and the semaphore, frees the block */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwInt32 rwaFileClose_005bfbf0(RwaAsyncFile *file)
{
  WaitForSingleObject(g_data_7dbc44, 0xffffffff);
  rwa_async_queue_remove_005bf6b0_r(&g_data_7dbc48, file);
  ReleaseSemaphore(g_data_7dbc44, 1, 0);
  rwa_file_close_00560e70(file->fp);
  CloseHandle(file->done);
  return RwaFreeListFree(&g_data_7dbc74, file);
}

/* drops the file's queued requests */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool rwa_async_file_cancel_005bf8f0(void *file)
{
  WaitForSingleObject(g_data_7dbc44, 0xffffffff);
  rwa_async_queue_remove_005bf6b0_r(&g_data_7dbc48, file);
  return ReleaseSemaphore(g_data_7dbc44, 1, 0);
}

/* rwa_async_queue_init_005bf5e0_r: C -> original rwa_async_queue_init_005bf5e0 (registers esi) */
__declspec(naked) RwBool rwa_async_queue_init_005bf5e0_r(RwaAsyncQueue * queue)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call rwa_async_queue_init_005bf5e0
        pop esi
        ret
    }
}

/* rwa_async_queue_remove_005bf6b0_r: C -> original rwa_async_queue_remove_005bf6b0 (registers esi,edi) */
__declspec(naked) RwBool rwa_async_queue_remove_005bf6b0_r(RwaAsyncQueue * queue, void * request)
{
    __asm {
        push esi
        push edi
        mov esi, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call rwa_async_queue_remove_005bf6b0
        pop edi
        pop esi
        ret
    }
}

/* entry thunk at the original address of rwa_async_queue_destroy_005bfba0: register inputs (esi) -> natural C rwa_async_queue_destroy_005bfba0_n; ret 0 */
__declspec(naked) void rwa_async_queue_destroy_005bfba0(void)
{
    __asm {
        push esi
        call rwa_async_queue_destroy_005bfba0_n
        add esp, 4
        ret
    }
}
