/* rwa_wave_conv_get_target_size_005c4af0 (0x005c4af0): _rwaWaveConvGetTargetSize, size of a converted wave */
#include "mfl_tier2.h"

/*005c4930*/ extern int FUN_005c4930(void *src_format, void *dst_format, int size, int a4, int a5);      /* _rwaWaveConvGetOffsetLength */

/* The offset / length query with the two extra arguments 0. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_wave_conv_get_target_size_005c4af0(void *src_format, void *dst_format, int size)
{
  return FUN_005c4930(src_format, dst_format, size, 0, 0);
}
