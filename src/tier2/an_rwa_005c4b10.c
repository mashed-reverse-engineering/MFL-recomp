/* rwa_wave_conv_005c4b10 (0x005c4b10): wave conversion (_rwaWaveConv): convert a buffer between two wave formats through a chain of up to 4 converters */
#include <string.h>
#include "mfl_tier2.h"

#define NUM_CONVERTERS 0x33
#define CONV_RESAMPLES 1
#define RWA_HINT       0x10806

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
/* a converter: returns the target bytes (dst NULL: only the size, and *dstOffset for srcOffset) */
typedef RwUInt32 (*RwaWaveConvFn)(const RwaWaveFormat *src, const void *srcData, RwUInt32 srcOffset,
                                  const RwaWaveFormat *dst, void *dstData, RwUInt32 *dstOffset);
typedef struct RwaWaveConvEntry { const RwaWaveConv *conv; RwaWaveConvFn fn; } RwaWaveConvEntry;

/*005bcbf0*/ extern int RwaWaveFormatCompare(const RwaWaveFormat *a, const RwaWaveFormat *b);      /* != 0: equal */
/*005bcf00*/ extern int RwaWaveDefFormatWaveFormatCompare(const void *dataType, const RwaWaveFormat *format);      /* != 0: matches */
/*005bcea0*/ extern void _rwaWaveDefFormatCopyToWaveFormat(const void *uuid, RwaWaveFormat *format, int a3);
/*005bcd50*/ extern void _rwaWaveFormatFreeMiscData(RwaWaveFormat *format);
/*005bf130*/ extern void *FUN_005bf130(RwUInt32 count, RwUInt32 size, RwUInt32 hint);      /* RwCalloc */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005c4680*/ extern RwUInt32 rwa_unit249490_005c4680(const void *srcType, RwUInt32 srcRate, const void *dstType, RwUInt32 dstRate,
                                                  const RwaWaveConvEntry **path, const RwaWaveConvEntry **stack,
                                                  RwUInt32 maxDepth, RwUInt32 depth, RwUInt32 best);      /* _RwaWaveConvFindPath (tier 1) */
/*0064aad8*/ extern RwaWaveConvEntry g_rwa_wave_converters[NUM_CONVERTERS];

/* Same formats: a plain copy.  Otherwise each step converts the current buffer into the other one (they swap; sized by the
   step's size query, which is asked about the ORIGINAL source data and offset, the offset then following the steps); the last
   step's result goes to dstData.  The target's misc data is used by the last step.  Returns the bytes written, 0 on failure. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_wave_conv_005c4b10(const RwaWaveFormat *srcFmt, const void *srcData, RwUInt32 srcOffset, RwaWaveFormat *dstFmt, void *dstData)
{
  const RwaWaveConvEntry *path[4];
  const RwaWaveConvEntry *stack[4];
  RwaWaveFormat cur, next;
  const void *srcType, *dstType;
  char *in, *out, *t;
  RwUInt32 steps, i, n, inBytes, outBytes, offset, swapBytes;
  int ownMisc;

  if (RwaWaveFormatCompare(srcFmt, dstFmt)) {
    memcpy(dstData, srcData, srcFmt->length);
    return srcFmt->length;
  }
  out = 0;
  srcType = 0;
  for (i = 0; i < NUM_CONVERTERS; i++)
    if (RwaWaveDefFormatWaveFormatCompare(g_rwa_wave_converters[i].conv->srcType, srcFmt)) {
      srcType = g_rwa_wave_converters[i].conv->srcType;
      break;
    }
  dstType = 0;
  for (i = 0; i < NUM_CONVERTERS; i++)
    if (RwaWaveDefFormatWaveFormatCompare(g_rwa_wave_converters[i].conv->dstType, dstFmt)) {
      dstType = g_rwa_wave_converters[i].conv->dstType;
      break;
    }
  steps = rwa_unit249490_005c4680(srcType, srcFmt->sampleRate, dstType, dstFmt->sampleRate, path, stack, 4, 0, 0xffffffff);
  cur = *srcFmt;
  outBytes = 0;
  next.sampleRate = 0;      /* RwaWaveFormat init: every field, not the padding */
  next.dataType = 0;
  next.length = 0;
  next.bits = 0;
  next.channels = 0;
  next.miscData = 0;
  next.miscBytes = 0;
  next.flags = 0;
  next.align = 0;
  in = (char *)FUN_005bf130(srcFmt->length, 1, RWA_HINT);
  if (!in)
    return 0;
  inBytes = srcFmt->length;
  memcpy(in, srcData, inBytes);

  for (i = 0; i < steps; i++) {
    _rwaWaveDefFormatCopyToWaveFormat(path[i]->conv->dstType, &next, 1);
    ownMisc = 1;
    next.sampleRate = (path[i]->conv->flags & CONV_RESAMPLES) ? dstFmt->sampleRate : cur.sampleRate;
    next.length = path[i]->fn(&cur, srcData, srcOffset, &next, 0, &offset);
    if (next.length > outBytes) {
      if (out)
        FUN_005bf120(out);
      out = (char *)FUN_005bf130(next.length, 1, RWA_HINT);
      if (!out) {
        if (in)
          FUN_005bf120(in);
        return 0;
      }
      outBytes = next.length;
    }
    n = next.length;
    next.miscData = dstFmt->miscData;
    next.miscBytes = dstFmt->miscBytes;
    if (next.miscData)
      ownMisc = 0;
    if (path[i]->fn(&cur, in, 0, &next, out, 0) != n) {
      if (out)
        FUN_005bf120(out);
      if (in)
        FUN_005bf120(in);
      return 0;
    }
    if (i == steps - 1) {      /* hand the misc data back */
      dstFmt->miscData = next.miscData;
      dstFmt->miscBytes = next.miscBytes;
    } else if (ownMisc)
      _rwaWaveFormatFreeMiscData(&next);
    cur = next;
    t = in; in = out; out = t;
    swapBytes = inBytes; inBytes = outBytes; outBytes = swapBytes;
    srcOffset = offset;
  }
  memcpy(dstData, in, next.length);
  FUN_005bf120(in);
  FUN_005bf120(out);
  return next.length;
}
