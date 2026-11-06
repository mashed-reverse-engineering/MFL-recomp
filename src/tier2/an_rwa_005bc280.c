/* rwa_wave_def_is_streamed_005bc280 (0x005bc280): wave definitions: flag 8 of the flags byte at +0x54 */
#include "mfl_tier2.h"

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwUInt32 rwa_wave_def_is_streamed_005bc280(const char *def)
{
  return *(const unsigned char *)(def + 0x54) >> 3 & 1;
}
