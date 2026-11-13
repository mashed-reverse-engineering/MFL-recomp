/* RenderWare Audio (Criterion Software) wave conversion: _rwaWaveConvGetOffsetLength, MFL.exe 0x005c4930.  Written from the
 * MFL.exe disassembly (objdump); types as the sibling rwa_wave_conv_005c4b10 (an_rwa_005c4b10.c). */
#include "mfl_tier2.h"

#define NUM_CONVERTERS 0x33
#define CONV_RESAMPLES 1

typedef struct RwaWaveFormat {      /* 0x1c */
  RwUInt32 sampleRate;
  const void *dataType;
  RwUInt32 length;         /* bytes */
  unsigned char bits, channels;
  unsigned short pad0e;
  void *miscData;          /* +0x10 */
  RwUInt32 miscBytes;      /* +0x14 */
  unsigned char flags, align;
  unsigned short pad1a;
} RwaWaveFormat;

typedef struct RwaWaveConv { const void *srcType, *dstType; RwUInt32 flags; } RwaWaveConv;
typedef RwUInt32 (*RwaWaveConvFn)(const RwaWaveFormat *src, const void *srcData, RwUInt32 srcOffset,
                                  const RwaWaveFormat *dst, void *dstData, RwUInt32 *dstOffset);
typedef struct RwaWaveConvEntry { const RwaWaveConv *conv; RwaWaveConvFn fn; } RwaWaveConvEntry;

/*005bcbf0*/ extern int RwaWaveFormatCompare(const RwaWaveFormat *a, const RwaWaveFormat *b);      /* != 0: equal */
/*005bcf00*/ extern int RwaWaveDefFormatWaveFormatCompare(const void *dataType, const RwaWaveFormat *format);      /* != 0: matches */
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, RwaWaveFormat *format, int a3);
/*005c4680*/ extern RwUInt32 rwa_unit249490_005c4680(const void *srcType, RwUInt32 srcRate, const void *dstType, RwUInt32 dstRate,
                                                  const RwaWaveConvEntry **path, const RwaWaveConvEntry **stack,
                                                  RwUInt32 maxDepth, RwUInt32 depth, RwUInt32 best);      /* _RwaWaveConvFindPath (tier 1) */
/*0064aad8*/ extern RwaWaveConvEntry g_rwa_wave_converters[NUM_CONVERTERS];

/* The size (bytes) that converting srcFmt data at srcOffset into dstFmt gives, and the matching target offset in *dstOffset:
   each step of the converter chain is asked for its output size.  Same formats: srcFmt's length and the offset unchanged.
   Returns 0 when a step fails.
   When no converter chain exists the original returns its (uninitialised) frame words: the last-step result shares its slot with
   the path search's work stack (word 2) and the target offset is never written.  The frame below has the original's layout so
   those reads see the same words; nothing initialises them, as in the original. */
/* TIER2: unvalidated 2026-11-13 authority=diffemu */
RwUInt32 _rwaWaveConvGetOffsetLength(const RwaWaveFormat *srcFmt, const void *srcData, const RwaWaveFormat *dstFmt, RwUInt32 srcOffset,
                                     RwUInt32 *dstOffset)
{
  struct {                                      /* the original's frame (esp+0x10 .. +0x60 after its four pushes) */
    RwUInt32 offset;                            /* +0x00 a step's target offset */
    RwUInt32 steps;                             /* +0x04 */
    const RwaWaveConvEntry *path[4];            /* +0x08 */
    union {
      const RwaWaveConvEntry *stack[7];         /* +0x18 the path search's work stack ... */
      RwaWaveFormat next;                       /* ... then the step's target format (length = the step's result) */
    } u;
    RwaWaveFormat cur;                          /* +0x34 */
  } f;
  const void *srcType = 0, *dstType = 0;
  RwUInt32 i;

  if (RwaWaveFormatCompare(srcFmt, dstFmt)) {
    if (dstOffset)
      *dstOffset = srcOffset;
    return srcFmt->length;
  }
  for (i = 0; i < NUM_CONVERTERS; i++)
    if (RwaWaveDefFormatWaveFormatCompare(g_rwa_wave_converters[i].conv->srcType, srcFmt)) {
      srcType = g_rwa_wave_converters[i].conv->srcType;
      break;
    }
  for (i = 0; i < NUM_CONVERTERS; i++)
    if (RwaWaveDefFormatWaveFormatCompare(g_rwa_wave_converters[i].conv->dstType, dstFmt)) {
      dstType = g_rwa_wave_converters[i].conv->dstType;
      break;
    }
  f.steps = rwa_unit249490_005c4680(srcType, srcFmt->sampleRate, dstType, dstFmt->sampleRate, f.path, f.u.stack, 4, 0, 0xffffffff);
  f.cur = *srcFmt;
  for (i = 0; i < f.steps; i++) {
    _rwaWaveDefFormatCopyToWaveFormat(f.path[i]->conv->dstType, &f.u.next, 1);
    f.u.next.sampleRate = (f.path[i]->conv->flags & CONV_RESAMPLES) ? dstFmt->sampleRate : f.cur.sampleRate;
    f.u.next.length = f.path[i]->fn(&f.cur, srcData, srcOffset, &f.u.next, 0, &f.offset);
    if (!f.u.next.length)
      return 0;
    srcOffset = f.offset;
    f.cur = f.u.next;
  }
  if (dstOffset)
    *dstOffset = f.offset;
  return f.u.next.length;
}
