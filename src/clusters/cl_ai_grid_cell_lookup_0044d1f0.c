// cluster-entries: ai_grid_cell_lookup_0044d1f0
/* Installed cluster ai_grid_cell_lookup_0044d1f0 (tools/cluster_install.py from output/cluster_pilot_zy/ai_grid_cell_lookup_0044d1f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007f0284*/ extern struct AiData g_ai_data;

/* Danger byte of the AI grid at world position (x, y) (4 cells per unit, rounded half away from zero); 0xff outside the grid / for an empty cell. */
unsigned char ai_grid_cell_lookup_0044d1f0(float x, float y)
{
  float fy = y * 4.0f;
  float fx = x * 4.0f;
  int iy = (int)(fy > 0.0f ? fy + 0.5f : fy - 0.5f);
  int ix = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f);
  short cell = g_ai_data.grid[((iy + 0x1f0) >> 3) * 0x80 + ((ix + 0x1f0) >> 3)];

  if (cell > 0 && cell < 0x200) {
    return g_ai_data.tile_pool[((ix & 7) + cell * 8) * 8 + (iy & 7)];
  }
  return 0xff;
}

