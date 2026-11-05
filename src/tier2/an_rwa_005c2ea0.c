/* rwa_stream_source_flush_005c2ea0 (0x005c2ea0): stream source: cancel the pending reads and return every block of the ring to free */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c9500*/ extern void _rwaStreamFileRequestQueueEmpty(void *queue);
/*005bbd60*/ extern void thunk__rwaRwStreamCancel_5bbd60(void *stream);
/*005c28c0*/ extern char *rwa_stream_source_get_block_005c28c0(void *source, void *block);
/*005c2a70*/ extern void __rwaStreamSourceUnlockStreamBlock(void *source, char *data);

#define BLOCK_FREE    1      /* block states (+0x10) */
#define BLOCK_LOADING 8
#define SRC_RELOAD    0x2000 /* source flags (+0x188) */
#define SRC_OPEN      0x8000
#define SRC_RING2     0x10000

typedef struct RwaStreamBlock { RwUInt32 filePos, frame, marker, newMarker, state; } RwaStreamBlock;      /* 0x14 */

typedef struct RwaStreamSource {
  char pad0[0x28];
  char stream[0x64];       /* +0x28 RwStream */
  char requests[0x60];     /* +0x8c file request queue */
  char padec[0x5c];
  RwaStreamBlock *blocks;  /* +0x148 */
  RwUInt32 writeBlock;     /* +0x14c */
  RwUInt32 loadBlock;      /* +0x150 oldest block still loading */
  RwUInt32 readBlock;      /* +0x154 */
  RwUInt32 playBlock;      /* +0x158 */
  RwUInt32 markBlock;      /* +0x15c */
  char pad160[4];
  RwUInt32 numBlocks;      /* +0x164 */
  char pad168[4];
  RwaStreamBlock *blocks2; /* +0x16c */
  char pad170[4];
  RwUInt32 block2;         /* +0x174 */
  char pad178[0x10];
  RwUInt32 flags;          /* +0x188 */
  void *lock;              /* +0x18c */
} RwaStreamSource;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_flush_005c2ea0(RwaStreamSource *s)
{
  RwaStreamSource *src = s;      /* the original hands the block out-pointer the slot of the argument */
  char *data;
  RwUInt32 i;

  if (!(src->flags & SRC_OPEN))
    return;
  _rwaStreamFileRequestQueueEmpty(src->requests);
  thunk__rwaRwStreamCancel_5bbd60(src->stream);
  src->flags &= ~0x28u;
  WaitForSingleObject(src->lock, 0xffffffff);
  src->flags |= SRC_RELOAD;
  while (src->blocks[src->loadBlock].state & BLOCK_LOADING) {      /* the cancelled loads */
    src->blocks[src->loadBlock].state = BLOCK_FREE;
    src->loadBlock++;
    if (src->loadBlock == src->numBlocks)
      src->loadBlock = 0;
  }
  ReleaseSemaphore(src->lock, 1, 0);
  while ((data = rwa_stream_source_get_block_005c28c0(src, &s)) != 0)      /* drain the loaded ones */
    __rwaStreamSourceUnlockStreamBlock(src, data);
  if (src->flags & SRC_RING2)
    for (i = 0; i < src->numBlocks; i++)
      src->blocks[i].state = BLOCK_FREE;
  if (src->blocks2)
    src->blocks2[src->block2].newMarker = 0;
  src->readBlock = 0;
  src->playBlock = 0;
  src->loadBlock = 0;
  src->markBlock = 0;
  src->writeBlock = 0;
  src->flags &= ~SRC_RELOAD;
}
