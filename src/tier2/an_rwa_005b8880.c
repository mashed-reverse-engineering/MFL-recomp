/* rwa_stream_execute_005b8880 (0x005b8880): streams: the stream object's execute callback: run the container, and on the first run after a pending start (flag 1, container state >= 5) set up its chains, start the stream object and move it to the running list */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c1980*/ extern int _rwaStreamContainerExecute(void *container);
/*005bfdc0*/ extern void *_rwaStreamContainerEnumRenderChains(void *container, void *callback, void *data);
/*005bfe40*/ extern void *_rwaStreamContainerEnumSourceChain(void *chain, void *callback, void *data);
/*005b8840*/ extern void *rwa_unit23fad0_005b8840(void *chain, void *stream);      /* chain enumeration callback */
/*005bac70*/ extern void *_rwaObjStartExecute(void *obj, int flag);
/*005bace0*/ extern void *_rwaObjFinishExecute(void *obj, int flag);
/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */
/*007dac24*/ extern struct RwaLink *g_data_7dac24;      /* running streams list head */

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;

#define STREAM_LINK(s)      ((RwaLink *)((s) + 0x14))
#define STREAM_CONTAINER(s) ((s) + 0xec)
#define STREAM_FLAGS(s)     (*(RwUInt32 *)((s) + 0xd9c))
#define STREAM_PENDING_START 0x1

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_stream_execute_005b8880(char *stream)
{
  char *container = STREAM_CONTAINER(stream);
  RwaLink *link = STREAM_LINK(stream);
  void *chain;

  _rwaStreamContainerExecute(container);
  if ((STREAM_FLAGS(stream) & STREAM_PENDING_START) && *(int *)(stream + 0xf4) >= 5) {
    chain = _rwaStreamContainerEnumRenderChains(container, rwa_unit23fad0_005b8840, stream);
    if (chain)
      _rwaStreamContainerEnumSourceChain(chain, rwa_unit23fad0_005b8840, stream);
    _rwaObjStartExecute(stream, 1);
    _rwaObjFinishExecute(stream, 1);

    WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff);
    if (link->next) {      /* unlink */
      *link->prev = link->next;
      link->next->prev = link->prev;
      link->prev = 0;
      link->next = 0;
    }
    link->next = g_data_7dac24;      /* insert at the head of the running list */
    link->prev = &g_data_7dac24;
    g_data_7dac24->prev = (RwaLink **)link;
    g_data_7dac24 = link;
    STREAM_FLAGS(stream) &= ~STREAM_PENDING_START;
    ReleaseSemaphore(rwastream_objDefRegistered, 1, 0);
  }
  return stream;
}
