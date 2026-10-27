#include "mfl_tier2.h"
/*0089de0c*/ extern undefined4 g_gfx_89de0c;

#define ATOMIC_PLUGIN_FLAGS_OFFSET 0x20 /* flags word inside the atomic plugin data */


/* atomic_plugin_get_flags */

/* "plugin data +0x20 of the atomic" */

/* Read the flags word of the atomic at offset atomic_offset. */
undefined4 __cdecl atomic_plugin_get_flags(char *atomic_offset)
{
  return *(undefined4 *)(g_gfx_89de0c + ATOMIC_PLUGIN_FLAGS_OFFSET + atomic_offset);
}
