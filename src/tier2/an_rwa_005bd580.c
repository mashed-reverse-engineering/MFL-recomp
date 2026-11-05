/* rwa_wave_format_get_samples_005bd580 (0x005bd580): wave formats (RwaWaveFormatGetSamples): the number of samples (per channel, unless interleaved) in a byte count */
#include "mfl_tier2.h"

/*005bd090*/ extern RwUInt32 RwaWaveFormatGetSamplesPerAudioFrame(const void *format);
/*005bcc50*/ extern RwUInt32 RwaWaveFormatGetFrameSize(const void *format);
/*005be600*/ extern int RwaUUIDCompare(const void *a, const void *b);      /* 0: equal */
/*005f7954*/ extern undefined g_data_5f7954;      /* rwaWAVEFORMAT_XADPCM */
/*005f79b4*/ extern undefined g_data_5f79b4;      /* rwaWAVEFORMAT_IMAADPCM */

typedef struct RwaWaveFormat {      /* 0x1c */
  RwUInt32 sampleRate;      /* +0x00 */
  const void *dataType;     /* +0x04 uuid */
  RwUInt32 length;          /* +0x08 */
  unsigned char bitsPerSample, channels, b0e, b0f;      /* +0x0c */
  void *extra;              /* +0x10 */
  RwUInt32 extraBytes;      /* +0x14 */
  unsigned char flags, b19, b1a, b1b;      /* +0x18 */
} RwaWaveFormat;

#define WF_INTERLEAVED 0x4      /* format flag: the count is over all channels */
#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_wave_format_get_samples_005bd580(const RwaWaveFormat *format, RwUInt32 bytes)
{
  RwUInt32 perFrame = RwaWaveFormatGetSamplesPerAudioFrame(format);
  RwUInt32 frame = RwaWaveFormatGetFrameSize(format);
  RwUInt32 samples;

  if ((!RwaUUIDCompare(format->dataType, &g_data_5f7954) || !RwaUUIDCompare(format->dataType, &g_data_5f79b4)) &&
      format->channels == 2) {
    float n = (float)bytes / (float)frame * 7.111111f;      /* stereo ADPCM: 64 samples per 9 frame bytes */

    samples = CHOP(n);
  } else
    samples = bytes / frame * perFrame;
  return samples / ((format->flags & WF_INTERLEAVED) ? 1 : format->channels);
}
