/* rwa_soft_wave_write_005da520 (0x005da520): software mixer wave: copy sample bytes into the wave's block */
#include <string.h>
#include "mfl_tier2.h"

/* wave+0x4c = sample block (2 guard bytes after the data, see rwa_soft_wave_alloc_data_005da4d0), +0x34 = data bytes,
   +0x38 = bits per sample, +0x39 = channels, +0x45 != 0 = looping wave.  For a one-shot wave the first 16-bit sample of
   each channel is copied behind the data (the interpolating mixer reads one sample ahead).  The guard index is the
   frame count in 16-bit units, as the original computes it. */
/* TIER2: unvalidated 2026-11-10 authority=static */
char *rwa_soft_wave_write_005da520(char *wave, RwUInt32 offset, const void *src, RwUInt32 bytes)
{
  short *data = *(short **)(wave + 0x4c);
  RwUInt32 frames;

  memcpy((char *)data + offset, src, bytes);
  if (*(unsigned char *)(wave + 0x45) == 0) {
    frames = *(RwUInt32 *)(wave + 0x34) / (RwUInt32)(*(unsigned char *)(wave + 0x38) >> 3) / *(unsigned char *)(wave + 0x39);
    switch (*(unsigned char *)(wave + 0x39)) {
    case 2:
      data[frames + 1] = data[1];
      /* fall through */
    case 1:
      data[frames] = data[0];
      break;
    }
  }
  return wave;
}
