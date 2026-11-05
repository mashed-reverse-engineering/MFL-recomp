/* rwa_stream_source_seek_005c2c90 (0x005c2c90): stream source (__rwaStreamSourceSeek): restart reading at a byte position of one sub stream */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005bf4e0*/ extern void RwaMul32By32(RwUInt32 a, RwUInt32 b, RwUInt32 *hi, RwUInt32 *lo);
/*005c93d0*/ extern int _rwaStreamFileRequestQueueAdd(void *queue, void *buffer, RwUInt32 bytes, RwUInt32 a4, RwUInt32 a5, RwUInt32 a6);
/*005c2bd0*/ extern RwUInt32 _RwaStreamSourceSeekReadCallback(void *source);

#define WF_INTERLEAVED_BLOCKS 4      /* wave format flags (+0x18): blocks hold all channels */

typedef struct RwaWaveFormatHdr { char pad0[0xd]; unsigned char channels; char pade[0xa]; unsigned char flags; } RwaWaveFormatHdr;

typedef struct RwaStreamFormatEntry {      /* 0x28 */
  char pad0[0x14];
  RwaWaveFormatHdr *wave;  /* +0x14 */
  char pad18[2];
  unsigned short blockBytes;      /* +0x1a per channel */
  char pad1c[4];
  RwUInt32 bytes;          /* +0x20 bytes of this sub stream per frame */
  char pad24[4];
} RwaStreamFormatEntry;

typedef struct RwaStreamFormat {
  char pad0[0x14];
  RwUInt32 numFormats;     /* +0x14 */
  RwaStreamFormatEntry *formats;      /* +0x18 */
  char pad1c[4];
  RwUInt32 frameBytes;     /* +0x20 */
  RwUInt32 dataOffset;     /* +0x24 */
} RwaStreamFormat;

typedef struct RwaStreamSource {
  char pad0[0x8c];
  char requests[0x60];     /* +0x8c file request queue */
  RwUInt32 endFrame;       /* +0xec */
  char padf0[4];
  RwUInt32 nextMarker;     /* +0xf4 */
  RwUInt32 framesLeft;     /* +0xf8 */
  RwUInt32 framesLeft2;    /* +0xfc */
  RwUInt32 frames;         /* +0x100 */
  char pad104[0x14];
  RwaStreamFormat *format; /* +0x118 */
  char pad11c[0x20];
  char *readFile;          /* +0x13c */
  char *file;              /* +0x140: +0x18 bytes, +0x1c start offset */
  char pad144[0x48];
  void *lock;              /* +0x18c */
} RwaStreamSource;

/* frame = pos / sub stream bytes; the position inside the frame is rounded up to whole blocks and turned into the marker
   (blocks * (2^32-1) / blocks per frame, +1 when above 1).  Returns 1, 0 when the read request fails. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_source_seek_005c2c90(RwaStreamSource *s, RwUInt32 sub, RwUInt32 pos)
{
  RwaStreamFormatEntry *e = &s->format->formats[sub];
  RwUInt32 frame, hi, lo, rem, rem2, ch, block, n, k;
  RwaStreamFormat *fmt;

  frame = pos / e->bytes;
  rem = pos;
  RwaMul32By32(frame, e->bytes, &pos, &sub);      /* the original reuses the dead argument slots for the 64-bit product */
  rem -= sub;
  rem2 = rem;
  ch = (e->wave->flags & WF_INTERLEAVED_BLOCKS) ? 1 : e->wave->channels;
  if (rem % (e->blockBytes * ch))
    rem = (e->blockBytes * ch + rem - 1) / (e->blockBytes * ch) * e->blockBytes * ch;
  block = e->blockBytes * ch;
  lo = n = rem / block;
  RwaMul32By32(n, 0xffffffff / (e->bytes / block), &hi, &lo);      /* lo: marker = blocks * (2^32-1) / blocks per frame */
  if (lo & ~1u)
    lo++;
  fmt = s->format;
  for (k = 0; k < fmt->numFormats; k++)      /* dead result (the original rounds this copy for every sub stream, then drops it) */
    if (rem2 % (fmt->formats[k].blockBytes * ch))
      rem2 = (fmt->formats[k].blockBytes * ch + rem2 - 1) / (fmt->formats[k].blockBytes * ch) * (fmt->formats[k].blockBytes * ch);

  WaitForSingleObject(s->lock, 0xffffffff);
  if (!_rwaStreamFileRequestQueueAdd(s->requests, 0, 0, fmt->frameBytes * frame + fmt->dataOffset + *(RwUInt32 *)(s->file + 0x1c), 0, 1)) {
    ReleaseSemaphore(s->lock, 1, 0);
    return 0;
  }
  if (s->readFile != s->file) {
    s->readFile = s->file;
    s->endFrame = _RwaStreamSourceSeekReadCallback(s);
  }
  s->nextMarker = lo;
  s->frames = *(RwUInt32 *)(s->readFile + 0x18) / s->format->frameBytes;
  s->framesLeft = s->frames - frame;
  s->framesLeft2 = s->frames - frame;
  ReleaseSemaphore(s->lock, 1, 0);
  return 1;
}
