/* RenderWare Audio (Criterion Software), MFL.exe: stream cache module close (0x005bb850), wave definition list destroy (0x005bd200),
 * RwaHeapDestroy (0x005bea00), file request queue empty (0x005c9500).  Written from the MFL.exe disassembly (tools/disasm_va.py);
 * names from the rwa.dll export / similarity rows of 2026-11-03. */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) RwBool __stdcall CloseHandle(void *handle);

typedef struct RwaDLink { struct RwaDLink *next, *prev; } RwaDLink;

typedef struct RwaHeap {
  RwUInt32 flags;               /* bit 0: static (not from the heap free list) */
  RwUInt32 blockSize, hint;
  RwaDLink blocks;              /* +0xc: block list head */
  RwaDLink link;                /* +0x14: in the list of all heaps */
} RwaHeap;

typedef struct RwaFileRequestQueue {
  RwUInt16 w00, w02, w04;
  RwUInt16 size;                /* +6 */
  RwUInt16 free;                /* +8 */
  RwUInt16 used;                /* +0xa */
  void *lock;                   /* +0xc semaphore */
} RwaFileRequestQueue;

/*005bcdd0*/ extern int __cdecl FUN_005bcdd0(void *a1);      /* RwaWaveDefDestroy */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);

/*007db770*/ extern void *g_data_7db770;      /* stream cache thread handle */
/*007daf48*/ extern void *g_data_7daf48;      /* stream cache semaphores */
/*007daf4c*/ extern void *g_data_7daf4c;
/*007dbb98*/ extern RwUInt8 g_data_7dbb98[];  /* heap free list */

/* _rwaRwStreamCacheCloseModule: wait for the cache thread to end, then take both cache semaphores and close them.  Returns the last CloseHandle result. */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_cache_close_module_005bb850(void)
{
  WaitForSingleObject(g_data_7db770, 0xffffffff);
  CloseHandle(g_data_7db770);
  WaitForSingleObject(g_data_7daf4c, 0xffffffff);
  WaitForSingleObject(g_data_7daf48, 0xffffffff);
  CloseHandle(g_data_7daf4c);
  return CloseHandle(g_data_7daf48);
}

/* destroy every wave definition of the list at owner+0x20 (link at +0x40 in the definition); list callback, returns NULL */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwa_wave_def_list_destroy_005bd200(void *data, char *owner)
{
  RwaDLink *head = (RwaDLink *)(owner + 0x20), *l = head->next, *next;

  while (l != head) {
    next = l->next;
    FUN_005bcdd0((char *)l - 0x40);
    l = next;
  }
  return 0;
}

/* RwaHeapDestroy: free every block, take the heap out of the heap list, give a dynamic heap back to the free list */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void rwa_heap_destroy_005bea00(RwaHeap *heap)
{
  RwaDLink *b;

  while ((b = heap->blocks.next) != &heap->blocks) {
    b->prev->next = b->next;
    b->next->prev = b->prev;
    FUN_005bf120(b);
  }
  heap->link.prev->next = heap->link.next;
  heap->link.next->prev = heap->link.prev;
  if (!(heap->flags & 1))
    RwaFreeListFree(g_data_7dbb98, heap);
}

/* _rwaStreamFileRequestQueueEmpty: all entries free again (under the queue lock) */
/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool rwa_stream_file_request_queue_empty_005c9500(RwaFileRequestQueue *q)
{
  WaitForSingleObject(q->lock, 0xffffffff);
  q->free = q->size;
  q->used = 0;
  return ReleaseSemaphore(q->lock, 1, 0);
}
