/* rwa_stream_source_read_005c3200 (0x005c3200): stream source (__rwaStreamSourceRead): queue file reads for the free blocks of the ring, looping at the end when the source loops */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c2c90*/ extern int __rwaStreamSourceSeek(void *source, RwUInt32 a2, RwUInt32 a3);
/*005c93d0*/ extern int _rwaStreamFileRequestQueueAdd(void *queue, void *buffer, RwUInt32 bytes, RwUInt32 a4, RwUInt32 a5, RwUInt32 a6);

#define BLOCK_FREE    1      /* block states (+0x10) */
#define BLOCK_LOADING 8
#define BLOCK_LAST    0x10   /* or'ed into the last block of a non-looping end */
#define SRC_LOOP      0x4000 /* source flags (+0x188) */

#define READ_OK       1      /* results */
#define READ_ERROR    2
#define READ_FULL     3
#define READ_END      4

typedef struct RwaStreamBlock {      /* 0x14 */
  RwUInt32 filePos;      /* +0x00 */
  RwUInt32 frame;        /* +0x04 frame number of the block */
  RwUInt32 marker;       /* +0x08 */
  RwUInt32 newMarker;    /* +0x0c the marker when it changed with this block, else 0 */
  RwUInt32 state;        /* +0x10 */
} RwaStreamBlock;

typedef struct RwaStreamSource {
  char pad0[0x8c];
  char requests[0x60];   /* +0x8c file request queue */
  RwUInt32 endFrame;     /* +0xec -1 = none */
  RwUInt32 marker;       /* +0xf0 */
  RwUInt32 nextMarker;   /* +0xf4 */
  RwUInt32 framesLeft;   /* +0xf8 */
  char padfc[4];
  RwUInt32 frames;       /* +0x100 */
  char pad104[0x14];
  char *format;          /* +0x118: +0x20 block bytes */
  char pad11c[0x20];
  RwUInt32 filePos;      /* +0x13c */
  char pad140[4];
  char *buffer;          /* +0x144 */
  RwaStreamBlock *blocks;      /* +0x148 */
  RwUInt32 writeBlock;   /* +0x14c */
  char pad150[4];
  RwUInt32 readBlock;    /* +0x154 */
  char pad158[8];
  RwUInt32 bufferBytes;  /* +0x160 */
  RwUInt32 numBlocks;    /* +0x164 */
  char pad168[0x18];
  RwUInt32 maxRead;      /* +0x180 blocks per request */
  RwUInt32 minFree;      /* +0x184 reserve */
  RwUInt32 flags;        /* +0x188 */
  void *lock;            /* +0x18c semaphore */
} RwaStreamSource;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_source_read_005c3200(RwaStreamSource *s)
{
  RwUInt32 blockBytes, free, n, bytes, offset, first, second, done, i;
  RwaStreamBlock *b;
  int result;

  WaitForSingleObject(s->lock, 0xffffffff);
  while (s->blocks[s->writeBlock].state == BLOCK_FREE) {
    blockBytes = *(RwUInt32 *)(s->format + 0x20);
    if (s->writeBlock == s->readBlock)
      free = s->numBlocks;
    else if (s->writeBlock > s->readBlock)
      free = s->numBlocks - s->writeBlock + s->readBlock;
    else
      free = s->readBlock - s->writeBlock;
    if (free < s->numBlocks - s->minFree)
      break;      /* not enough free blocks yet */

    if (s->framesLeft) {
      result = READ_OK;
      n = s->maxRead;
      if (n > free)
        n = free;
      if (n > s->framesLeft)
        n = s->framesLeft;
      bytes = n * blockBytes;
      offset = s->writeBlock * blockBytes;
      first = s->bufferBytes - offset;      /* up to the end of the ring, the rest from its start */
      if (first > bytes)
        first = bytes;
      second = bytes - first;
      done = 0;
      if (first) {
        if (!_rwaStreamFileRequestQueueAdd(s->requests, s->buffer + offset, first, 0, 0, 4)) {
          ReleaseSemaphore(s->lock, 1, 0);
          return READ_ERROR;
        }
        done = first;
      }
      if (second) {
        if (_rwaStreamFileRequestQueueAdd(s->requests, s->buffer, second, 0, 0, 4))
          done += second;
        else
          result = READ_ERROR;
      }
      while (done) {
        b = &s->blocks[s->writeBlock];
        b->frame = s->frames - s->framesLeft;
        b->filePos = s->filePos;
        b->state = BLOCK_LOADING;
        if (s->marker != s->nextMarker) {
          b->newMarker = s->nextMarker;
          s->marker = s->nextMarker;
        } else
          b->newMarker = 0;
        b->marker = s->marker;
        done -= blockBytes;
        s->framesLeft--;
        i = s->writeBlock + 1;
        if (i == s->numBlocks)
          i = 0;
        s->writeBlock = i;
      }
      if (!s->framesLeft && s->endFrame != 0xffffffff && (s->flags & SRC_LOOP)) {
        i = s->writeBlock;
        if (!i)
          i = s->numBlocks;
        s->blocks[i - 1].state |= BLOCK_LAST;
      }
      ReleaseSemaphore(s->lock, 1, 0);
      return result;
    }

    ReleaseSemaphore(s->lock, 1, 0);      /* at the end: loop back to the start, or stop */
    if (!(s->flags & SRC_LOOP))
      return READ_END;
    if (!__rwaStreamSourceSeek(s, 0, 0))
      return 0;
    WaitForSingleObject(s->lock, 0xffffffff);
  }
  ReleaseSemaphore(s->lock, 1, 0);
  return READ_FULL;
}
