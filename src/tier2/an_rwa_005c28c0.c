/* rwa_stream_source_get_block_005c28c0 (0x005c28c0): stream source: take the next loaded block of the ring (or of the second ring while that is active) for playback */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);

#define BLOCK_LOADED  2      /* block states (+0x10) */
#define BLOCK_PLAYING 4
#define SRC_RELOAD    0x2000 /* source flags (+0x188): a loaded block may be taken again */
#define SRC_RING2     0x10000

typedef struct RwaStreamBlock { RwUInt32 filePos, frame, marker, newMarker, state; } RwaStreamBlock;      /* 0x14 */

typedef struct RwaStreamSource {
  char pad0[0xfc];
  RwUInt32 framesLeft2;    /* +0xfc blocks not yet played */
  char pad100[0x18];
  char *format;            /* +0x118: +0x20 block bytes */
  char pad11c[0x28];
  char *buffer;            /* +0x144 */
  RwaStreamBlock *blocks;  /* +0x148 */
  char pad14c[0xc];
  RwUInt32 playBlock;      /* +0x158 */
  RwUInt32 markBlock;      /* +0x15c follows playBlock when equal */
  char pad160[4];
  RwUInt32 numBlocks;      /* +0x164 */
  char *buffer2;           /* +0x168 */
  RwaStreamBlock *blocks2; /* +0x16c */
  char pad170[8];
  RwUInt32 playBlock2;     /* +0x178 */
  RwUInt32 numBlocks2;     /* +0x17c */
  char pad180[8];
  RwUInt32 flags;          /* +0x188 */
  void *lock;              /* +0x18c */
} RwaStreamSource;

#define TAKEABLE(state, flags) (((flags) & SRC_RELOAD) ? ((state) & BLOCK_LOADED) : (((state) & BLOCK_LOADED) && !((state) & BLOCK_PLAYING)))

/* Returns the block's data (*block = its record, now playing), NULL when the next block is not loaded. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_stream_source_get_block_005c28c0(RwaStreamSource *s, RwaStreamBlock **block)
{
  RwUInt32 blockBytes, state, i, old;
  char *data;

  WaitForSingleObject(s->lock, 0xffffffff);
  blockBytes = *(RwUInt32 *)(s->format + 0x20);
  if (s->flags & SRC_RING2) {
    state = s->blocks2[s->playBlock2].state;
    if (!TAKEABLE(state, s->flags)) {
      ReleaseSemaphore(s->lock, 1, 0);
      return 0;
    }
    data = s->playBlock2 * blockBytes + s->buffer2;
    *block = &s->blocks2[s->playBlock2];
    s->blocks2[s->playBlock2].state = BLOCK_PLAYING;
    i = s->playBlock2 + 1;
    if (i == s->numBlocks2)
      i = 0;
    s->playBlock2 = i;
  } else {
    state = s->blocks[s->playBlock].state;
    if (!TAKEABLE(state, s->flags)) {
      ReleaseSemaphore(s->lock, 1, 0);
      return 0;
    }
    data = s->playBlock * blockBytes + s->buffer;
    *block = &s->blocks[s->playBlock];
    s->blocks[s->playBlock].state = BLOCK_PLAYING;
    old = s->playBlock;
    i = old + 1;
    if (i == s->numBlocks)
      i = 0;
    s->playBlock = i;
    if (s->markBlock == old)
      s->markBlock = i;
  }
  s->framesLeft2--;
  ReleaseSemaphore(s->lock, 1, 0);
  return data;
}
