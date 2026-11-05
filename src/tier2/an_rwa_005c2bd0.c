/* rwa_stream_source_frame_marker_005c2bd0 (0x005c2bd0): stream sources: the largest sub-frame marker of the sub-stream positions (inside a frame) */
#include "mfl_tier2.h"

/*005bf4e0*/ extern void RwaMul32By32(RwUInt32 a, RwUInt32 b, RwUInt32 *hi, RwUInt32 *lo);

/* source+0x118: format (+0x14 sub-stream count, +0x18 sub-streams of 0x28 bytes, +0x20 samples per frame);
   source+0x13c: state (+0x0c per sub-stream sample positions).  Only positions above the largest one so far count.
   Returns the largest marker (rem * (2^32-1) / frame, low word, + 1), 0 without one, -1 when that is 1. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_stream_source_frame_marker_005c2bd0(char *source)
{
  char *state = *(char **)(source + 0x13c);
  char *sub = *(char **)(*(char **)(source + 0x118) + 0x18) + 0x20;
  RwUInt32 best = 0, maxPos = 0, i;

  for (i = 0; i < *(RwUInt32 *)(*(char **)(source + 0x118) + 0x14); i++, sub += 0x28) {
    RwUInt32 *pos = *(RwUInt32 **)(state + 0xc);

    if (pos[i] > maxPos) {
      RwUInt32 frame = *(RwUInt32 *)sub, hi, lo;

      maxPos = pos[i];
      lo = maxPos % frame;      /* the original keeps the remainder in the product's low word slot */
      RwaMul32By32(lo, 0xffffffff / frame, &hi, &lo);
      lo++;
      if (lo > best)
        best = lo;
    }
  }
  if (best == 1)
    return (RwUInt32)-1;
  return best;
}
