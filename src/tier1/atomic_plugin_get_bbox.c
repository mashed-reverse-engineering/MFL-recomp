#include "mfl_tier2.h"
/*0089de0c*/ extern undefined4 g_gfx_89de0c;

#define ATOMIC_PLUGIN_BBOX_OFFSET 8 /* bounding box inside the atomic plugin data */


/* atomic_plugin_get_bbox */

/* "address of the plugin bbox (+8)" */

/* Address of the bounding box of the atomic at offset atomic_offset. */
int atomic_plugin_get_bbox(int atomic_offset)
{
  return g_gfx_89de0c + ATOMIC_PLUGIN_BBOX_OFFSET + atomic_offset;
}
