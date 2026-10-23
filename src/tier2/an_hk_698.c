#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*007f0284*/ extern struct AiData g_ai_data;
/*007f8284*/ extern char g_ai_cells[];

/* world position -> grid fixed point scale (inferred) */
#define AI_POS_SCALE (4.0f)
/* offset added to scaled positions so the grid index stays non-negative (inferred) */
#define AI_GRID_ORIGIN (0x1f0)
/* grid is AI_GRID_DIM x AI_GRID_DIM cells (grid[128][128]) */
#define AI_GRID_DIM (0x80)
/* scaled position >> AI_CELL_SHIFT = grid cell index (inferred: 8 unit cells) */
#define AI_CELL_SHIFT (3)
/* tile pool holds 512 tiles; valid tile index is 1..AI_TILE_COUNT-1 (0 and -1 = none) */
#define AI_TILE_COUNT (0x200)

/* ai_cell_is_value2 */

/* "danger data cell value == 2 at a world position" */

/* Returns 1 if the AI danger cell under the world position is value 2, else 0.
   pos: float[2] world position, pos[0] = x, pos[1] = y (inferred from index use).
   unused_param: not read by this function (inferred). */
undefined4 __cdecl ai_cell_is_value2(float *pos,char *unused_param)

{
  float y_scaled_f;
  float x_scaled_f;
  short tile_index;
  uint y_scaled;
  uint x_scaled;

  /* round scaled position to nearest integer, half away from zero */
  y_scaled_f = (*(float *)((char *)pos + 4) * AI_POS_SCALE);
  y_scaled = ((int)((y_scaled_f > 0.0f) ? (y_scaled_f + 0.5f) : (y_scaled_f - 0.5f)));
  x_scaled_f = (*(float *)((char *)pos + 0) * AI_POS_SCALE);
  x_scaled = ((int)((x_scaled_f > 0.0f) ? (x_scaled_f + 0.5f) : (x_scaled_f - 0.5f)));
  /* grid cell lookup; tile_index is the tile in the pool for that cell */
  tile_index = g_ai_data.grid[((int)(y_scaled + AI_GRID_ORIGIN) >> AI_CELL_SHIFT) * AI_GRID_DIM + ((int)(x_scaled + AI_GRID_ORIGIN) >> AI_CELL_SHIFT)];
  /* tile must be a real tile, and the 8x8 byte at the position's sub-cell offset must be 2 */
  if (((0 < tile_index) && (tile_index < AI_TILE_COUNT)) &&
     (*(char *)((y_scaled & 7) + ((char *)&g_ai_cells) + ((x_scaled & 7) + tile_index * 8) * 8) == '\x02')) {
    return 1;
  }
  return 0;
}


