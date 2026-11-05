/* rwa_stream_source_unlock_block_005c2a70 (0x005c2a70): stream source (__rwaStreamSourceUnlockStreamBlock): give a played block back to its ring and move the read position over the free blocks */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

#define BLOCK_FREE 1      /* block state (+0x10) */

typedef struct RwaStreamBlock { RwUInt32 filePos, frame, marker, newMarker, state; } RwaStreamBlock;      /* 0x14 */

typedef struct RwaStreamSource {
  char pad0[0x118];
  char *format;            /* +0x118: +0x20 block bytes */
  char pad11c[0x28];
  char *buffer;            /* +0x144 */
  RwaStreamBlock *blocks;  /* +0x148 */
  char pad14c[8];
  RwUInt32 readBlock;      /* +0x154 first block not free */
  RwUInt32 playBlock;      /* +0x158 */
  RwUInt32 markBlock;      /* +0x15c */
  RwUInt32 bufferBytes;    /* +0x160 */
  RwUInt32 numBlocks;      /* +0x164 */
  char *buffer2;           /* +0x168 */
  RwaStreamBlock *blocks2; /* +0x16c */
  char pad170[0x1c];
  void *lock;              /* +0x18c */
} RwaStreamSource;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_unlock_block_005c2a70(RwaStreamSource *s, char *data)
{
  RwUInt32 blockBytes, i;
  int first;

  WaitForSingleObject(s->lock, 0xffffffff);
  blockBytes = *(RwUInt32 *)(s->format + 0x20);
  if (data < s->buffer || data >= s->buffer + s->bufferBytes) {      /* a block of the second ring */
    s->blocks2[(RwUInt32)(data - s->buffer2) / blockBytes].state = BLOCK_FREE;
    ReleaseSemaphore(s->lock, 1, 0);
    return;
  }
  i = (RwUInt32)(data - s->buffer) / blockBytes;
  s->blocks[i].state = BLOCK_FREE;
  if (++i == s->numBlocks)
    i = 0;
  if (i == s->playBlock) {      /* everything played: restart at the mark */
    s->readBlock = s->markBlock;
    s->playBlock = s->readBlock;
    ReleaseSemaphore(s->lock, 1, 0);
    return;
  }
  i = s->readBlock;      /* skip the free blocks, at most one wrap */
  first = 1;
  while (s->blocks[i].state & BLOCK_FREE) {
    if (++i == s->numBlocks) {
      i = 0;
      if (!first)
        break;
      first = 0;
    }
  }
  s->readBlock = i;
  ReleaseSemaphore(s->lock, 1, 0);
}
