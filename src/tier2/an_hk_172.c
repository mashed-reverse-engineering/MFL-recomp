#include "mfl_tier2.h"
/*0064a998*/ extern char *g_data_64a998;
/*0064a99c*/ extern char *g_data_64a99c;
#define FRAME_DATA_BYTES (14)
#define HEADER_SCALE_MASK (0xf)
#define NIBBLE_TOP_SHIFT (0x1c) /* 32 - 4: moves a 4-bit nibble to the top bits so >> sign-extends it */
#define SHIFT_COUNT_MASK (0x1f)
#define FIXED_POINT_SHIFT (0x10)
/* FUN_005c7080 */
/* ADPCM decoder, N64 style (inferred). Decodes frame_count frames from adpcm_in into 16-bit PCM at pcm_out.
 * Frame layout (inferred): 16 bytes = header, skipped byte, then 14 data bytes holding 28 signed 4-bit samples.
 * Header byte: high nibble = coef index (picks the coef_0 / coef_1 pair), low nibble = scale (shift = 0x1c - scale).
 * Each sample = (nibble << shift) + hist_1 * coef_1 + hist_2 * coef_0, keeping the high 16 bits of the sum.
 * The second sample of each data byte uses coef_0 / coef_1 swapped, with hist_1 already set to the first sample.
 * hist_1 / hist_2 carry the last two samples across frames; hist_1 is parked in local_c between frames. */

void __cdecl FUN_005c7080(byte *adpcm_in,undefined2 *pcm_out,int frame_count)

{
  int coef_1;
  int coef_0;
  int acc;
  int hist_1;
  byte shift;
  int hist_2;
  undefined2 *out_pos;
  uint coef_index;
  int local_c;
  int frames_left;

  hist_2 = 0;
  local_c = 0;
  if (frame_count != 0) {
    frames_left = frame_count;
    /* one frame per pass: read its header (coef index, scale), skip a byte, then decode its 14 data bytes */
    do {
      hist_1 = local_c;
      coef_index = (uint)(*adpcm_in >> 4);
      coef_1 = *(int *)((char *)&g_data_64a99c + coef_index * 8);
      shift = 0x1c - (*adpcm_in & HEADER_SCALE_MASK);
      coef_0 = *(int *)((char *)&g_data_64a998 + coef_index * 8);
      adpcm_in = adpcm_in + 2;
      local_c = FRAME_DATA_BYTES;
      out_pos = pcm_out;
      /* one data byte per pass: low nibble -> first sample, high nibble -> second sample */
      do {
        pcm_out = out_pos + 2;
        acc = (((int)((uint)*adpcm_in << NIBBLE_TOP_SHIFT) >> NIBBLE_TOP_SHIFT) <<
               (shift & SHIFT_COUNT_MASK)) + hist_1 * coef_1 +
                hist_2 * coef_0;
        hist_1 = acc >> FIXED_POINT_SHIFT;
        *out_pos = (short)((uint)acc >> FIXED_POINT_SHIFT);
        acc = (((int)((uint)(*adpcm_in >> 4) << NIBBLE_TOP_SHIFT) >> NIBBLE_TOP_SHIFT) <<
               (shift & SHIFT_COUNT_MASK)) + hist_1 * coef_0 +
                hist_2 * coef_1;
        hist_2 = acc >> FIXED_POINT_SHIFT;
        out_pos[1] = (short)((uint)acc >> FIXED_POINT_SHIFT);
        adpcm_in = adpcm_in + 1;
        local_c = local_c + -1;
        out_pos = pcm_out;
      } while (local_c != 0);
      frames_left = frames_left + -1;
      local_c = hist_1;
    } while (frames_left != 0);
  }
  return;
}


