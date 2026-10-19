#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd628*/ extern float k_f_0_25;
/*005e64a4*/ extern float k_f_0;
/*007f0284*/ extern struct AiData g_ai_data;

#define AI_GRID_WIDTH (0x80) /* tiles per grid row, grid is [128][128] */
#define AI_GRID_BIAS (0x1f0) /* added to the cell coordinate before >> 3 (inferred) */
#define AI_TILE_COUNT (0x200) /* valid tile indices are 1 .. 511 */

/* ai_target_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: validates a candidate target (position, target)" */

/* Returns 1 if the straight segment from origin to target is clear, 0 if a sample point on it lands in a blocked cell (inferred).
 * origin, target: pointers to float x, y pairs (world positions).
 * Samples are taken every k_f_0_25 units along the segment (0.25, inferred from the name).
 * Each sample maps to a grid cell -> tile index in g_ai_data.grid -> 8x8 byte block in tile_pool.
 */

undefined4 __cdecl ai_target_check(float *origin,float *target)

{
  char cell_danger;
  short tile_index;
  float inv_length;
  float sample_x, sample_y, scaled_x, scaled_y;
  uint cell_x;
  uint cell_y;
  RwReal length;
  float walked;
  float *walked_ptr = &walked;
  RwV2d delta;

  delta.x = *target - *origin;
  walked = 0.0;
  delta.y = target[1] - origin[1];
  length = RwV2dLength(&delta);
  if (k_f_0 < length) {
    inv_length = float_1_0 / length;
    /* Walk the segment in 0.25 unit steps until walked reaches the full length. */
    do {
      sample_x = inv_length * delta.x * *walked_ptr + *origin;
      sample_y = inv_length * delta.y * *walked_ptr + origin[1];
      /* Samples that share x or y with origin are not tested. */
      if ((sample_x != *origin) && (sample_y != origin[1])) {
        /* Convert the sample to grid cell coordinates, then to a tile index.
         * Tile 0 and indices >= AI_TILE_COUNT (incl. negative) mean no tile.
         */
        scaled_y = sample_y * k_f_4;
        scaled_x = sample_x * k_f_4;
        cell_y = (int)(scaled_y > k_f_0 ? scaled_y + k_f_0_5 : scaled_y - k_f_0_5);
        cell_x = (int)(scaled_x > k_f_0 ? scaled_x + k_f_0_5 : scaled_x - k_f_0_5);
        tile_index = g_ai_data.grid[((int)(cell_y + AI_GRID_BIAS) >> 3) * AI_GRID_WIDTH + ((int)(cell_x + AI_GRID_BIAS) >> 3)];
        /* A byte of 0 or 3 in the tile blocks the line (inferred: 0xFF is empty per the dossier). */
        if (((0 < tile_index) && (tile_index < AI_TILE_COUNT)) &&
           ((cell_danger = *(char *)((cell_y & 7) + (int)g_ai_data.tile_pool + ((cell_x & 7) + tile_index * 8) * 8),
            cell_danger == '\0' || (cell_danger == '\x03')))) {
          return 0;
        }
      }
      walked = walked + k_f_0_25;
    } while (walked < length);
  }
  return 1;
}


