/* rwa_stream_source_set_stream_pos_005c8ae0 (0x005c8ae0): stream sources (_rwaStreamSourceSetStreamPos): select the segment and seek to a time / byte position, x87 */
#include "mfl_tier2.h"

/*005c2ea0*/ extern void rwa_stream_source_flush_005c2ea0(char *source);
/*005c2c90*/ extern int rwa_stream_source_seek_005c2c90(char *source, RwUInt32 sub, RwUInt32 pos);
/*005c3090*/ extern void __rwaStreamSourceSetSegmentByName(char *source, void *name);
/*005c30c0*/ extern void __rwaStreamSourceSetSegmentByUUID(char *source, void *uuid);

typedef struct RwaStreamPosSegment {
  union { void *id; RwUInt32 index; } segment;      /* +0x00 UUID / name, or index */
  RwUInt32 segmentType;    /* +0x04 */
  union { float seconds; RwUInt32 bytes; } pos;      /* +0x08 */
  RwUInt32 posType;        /* +0x0c */
} RwaStreamPosSegment;

typedef struct RwaStreamPos {
  RwaStreamPosSegment *pos;      /* +0x00 */
  RwUInt32 flush;                /* +0x04 */
} RwaStreamPos;

#define SEGMENT_UUID  1
#define SEGMENT_NAME  2
#define SEGMENT_INDEX 3
#define POS_TIME      1

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* obj: the stream source object (its source state at +0x5c: +0x118 format, +0x140 current segment).  The sub-stream
   format (+0x0c bytes per frame, +0x14 wave format: +0 sample rate, +0xd channels; +0x1a frame bytes) turns the time
   (rounded up to whole frames) or byte position into the seek position. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_source_set_stream_pos_005c8ae0(char *obj, RwaStreamPos *streamPos)
{
  char *source = obj + 0x5c;
  char *sub;
  RwaStreamPosSegment *p;
  RwUInt32 bytes, frame, pos;

  if (streamPos->flush)
    rwa_stream_source_flush_005c2ea0(source);
  sub = *(char **)(*(char **)(source + 0x118) + 0x18);
  p = streamPos->pos;
  frame = *(RwUInt32 *)(sub + 0xc);
  if (p->posType == POS_TIME) {
    float samples = (float)**(RwUInt32 **)(sub + 0x14) * p->pos.seconds;

    bytes = ((RwUInt32)CHOP(samples) + frame - 1) / frame * frame;
  } else
    bytes = p->pos.bytes;
  pos = bytes / frame * *(unsigned char *)(*(char **)(sub + 0x14) + 0xd) * *(unsigned short *)(sub + 0x1a);

  switch (p->segmentType) {
  case SEGMENT_UUID:
    __rwaStreamSourceSetSegmentByUUID(source, p->segment.id);
    break;
  case SEGMENT_NAME:
    __rwaStreamSourceSetSegmentByName(source, p->segment.id);
    break;
  case SEGMENT_INDEX: {
    char *segments = *(char **)(*(char **)(source + 0x118) + 0x10);      /* 0x20 bytes each */
    char *current = segments + p->segment.index * 0x20;

    *(char **)(source + 0x140) = current;
    break;
  }
  }
  return rwa_stream_source_seek_005c2c90(source, 0, pos);
}
