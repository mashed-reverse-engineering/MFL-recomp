// cluster-entries: _rwaRwStreamCacheReadAsync
/* Installed cluster _rwaRwStreamCacheReadAsync (tools/cluster_install.py from output/cluster_pilot_auw7/_rwaRwStreamCacheReadAsync.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RenderWare Audio (Criterion Software) buffered stream reads: MFL.exe 0x005bbbc0 _rwaRwStreamCacheReadAsync (register function:
   stream in edi, byte count in eax, then dest / callback / data on the stack) starts a buffered read: at once when the pending list
   is free, else it queues the request.  Written from the MFL.exe disassembly (objdump). */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

typedef void (*RwaAsyncCallback)(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);
typedef struct RwaPendLink { struct RwaPendLink *next, *prev; } RwaPendLink;
typedef struct RwaBufRead {        /* at stream + 4, as in an_rwa_stream_read_cb.c */
  RwaPendLink link;                /* +0x00 */
  RwUInt32 state;                  /* +0x08 1 done, 2 queued, 3 busy, 4 reading, 5 seeking */
  RwUInt32 position;               /* +0x0c */
  RwUInt32 w10;                    /* +0x10 bytes asked for */
  RwUInt32 remaining;              /* +0x14 */
  char *dest;                      /* +0x18 */
  void *file;                      /* +0x1c */
  RwUInt32 w20, w24, readOffset, requested;
  char *buffer;                    /* +0x30 */
  RwaAsyncCallback callback;       /* +0x34 */
  void *data;                      /* +0x38 */
} RwaBufRead;

/*007daf4c*/ extern void *g_data_7daf4c;          /* lock of the pending list */
/*007daf50*/ extern RwaPendLink g_data_7daf50;    /* pending requests */
/*007daf48*/ extern void *g_data_7daf48;          /* free while no request is being continued */
/*007daf58*/ extern RwaBufRead *g_data_7daf58;    /* request being continued */
/*005c4370*/ extern RwBool _rwaSemaphorePoll(void **semaphore);
/*005bb8a0*/ extern void FUN_005bb8a0(void *file, RwUInt32 done, RwUInt32 result, RwUInt32 op, void *data);      /* the read callback */

/* TIER2: unvalidated 2026-11-13 authority=diffemu */
void _rwaRwStreamCacheReadAsync_n(char *stream, RwUInt32 bytes, char *dest, RwaAsyncCallback callback, void *data)
{
  RwaBufRead *r = (RwaBufRead *)(stream + 4);

  r->state = 3;
  r->file = *(void **)(*(char **)stream + 0xc);
  r->w10 = bytes;
  r->remaining = bytes;
  r->dest = dest;
  r->callback = callback;
  r->data = data;
  WaitForSingleObject(g_data_7daf4c, 0xffffffff);
  if (_rwaSemaphorePoll(&g_data_7daf48)) {      /* nobody is being continued: run it now */
    g_data_7daf58 = r;
    ReleaseSemaphore(g_data_7daf4c, 1, 0);
    FUN_005bb8a0(r->file, r->w10, 0, 1, stream);
    return;
  }
  r->link.next = g_data_7daf50.next;      /* queue it */
  r->link.prev = &g_data_7daf50;
  g_data_7daf50.next->prev = &r->link;
  g_data_7daf50.next = &r->link;
  r->state = 2;
  ReleaseSemaphore(g_data_7daf4c, 1, 0);
}

/* entry thunk at the original address of _rwaRwStreamCacheReadAsync: register inputs (edi,eax,s0,s1,s2) -> natural C _rwaRwStreamCacheReadAsync_n; ret 0 */
__declspec(naked) void _rwaRwStreamCacheReadAsync(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push eax
        push edi
        call _rwaRwStreamCacheReadAsync_n
        add esp, 20
        ret
    }
}
