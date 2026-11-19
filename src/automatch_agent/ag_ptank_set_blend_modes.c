#include "mfl_tier2.h"
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;


/* ptank_set_blend_modes */

/* "(ptank atomic, src, dst): sets the blend modes and flag 0x10000000" */

void ptank_set_blend_modes(char *ptank,undefined4 src_blend,undefined4 dst_blend)

{
  *(undefined4 *)(*(char **)(g_ptank_plugin_offset + *(char **)(ptank + 4)) + 0xa4) = src_blend;
  *(undefined4 *)(*(char **)(g_ptank_plugin_offset + *(char **)(ptank + 4)) + 0xa8) = dst_blend;
  *(uint *)(*(char **)(g_ptank_plugin_offset + *(char **)(ptank + 4)) + 0x40) |= 0x10000000;
  return;
}
