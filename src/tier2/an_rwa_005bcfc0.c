/* rwa_wave_def_is_target_format_supported_005bcfc0 (0x005bcfc0): wave definitions (_rwaWaveDefIsTargetFormatSupported): does one of the definition's supported formats match (bits, channels, rate range, data type, interleave flag); a definition without a list takes everything */
#include "mfl_tier2.h"

/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0: equal */

typedef struct RwaWaveFormat {      /* 0x1c */
  RwUInt32 sampleRate;      /* +0x00 */
  const void *dataType;     /* +0x04 uuid */
  RwUInt32 length;          /* +0x08 */
  unsigned char bitsPerSample, channels, b0e, b0f;      /* +0x0c */
  void *extra;              /* +0x10 */
  RwUInt32 extraBytes;      /* +0x14 */
  unsigned char flags, b19, b1a, b1b;      /* +0x18 */
} RwaWaveFormat;

typedef struct RwaWaveCaps {      /* one supported format of a definition */
  const void *dataType;     /* +0x00 uuid */
  unsigned char bitsPerSample, channels, b06, b07;      /* +0x04 */
  RwUInt32 minRate, maxRate;      /* +0x08 / +0x0c */
  unsigned char flags;      /* +0x10 */
} RwaWaveCaps;

#define WAVEDEF_NCAPS(d) (*(RwUInt32 *)((d) + 0x24))
#define WAVEDEF_CAPS(d)  (*(RwaWaveCaps ***)((d) + 0x28))

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_wave_def_is_target_format_supported_005bcfc0(char *def, RwaWaveFormat *format)
{
  RwUInt32 n = WAVEDEF_NCAPS(def), i;
  RwaWaveCaps **caps;

  if (!n)
    return 1;
  caps = WAVEDEF_CAPS(def);
  for (i = 0; i < n; i++) {
    RwaWaveCaps *c = caps[i];

    if (format->channels == c->channels && format->bitsPerSample == c->bitsPerSample &&
        format->sampleRate >= c->minRate && format->sampleRate <= c->maxRate &&
        !RwaUUIDCompare(format->dataType, c->dataType) && !((c->flags ^ format->flags) & 1))
      return 1;
  }
  return 0;
}
