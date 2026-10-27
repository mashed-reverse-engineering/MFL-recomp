#include "mfl_tier2.h"
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;


#define MARKER_ENTRY_STRIDE 4 /* dwords per marker entry */

/* player_marker_get_clump */

/* Returns the clump field of the marker entry for the given player index. */

undefined4 player_marker_get_clump(int player_index)
{
  return (&g_gfx_6c3f3c)[player_index * MARKER_ENTRY_STRIDE];
}
