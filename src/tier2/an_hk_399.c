#include "mfl_tier2.h"
/*007f0284*/ extern struct AiData g_ai_data;
/*007f8284*/ extern char g_ai_cells[];
#define GRID_ORIGIN_OFFSET (0x200) /* world coord of cell 0 is -512 (half of 1024 units) */
#define GRID_CELLS (0x80) /* grid is 128 x 128 cells */
#define TILE_COUNT (0x200) /* 512 tiles in the pool, tile 0 is never allocated */
#define TILE_BYTES (0x40) /* 8 x 8 danger bytes per tile */
#define TILE_EMPTY (-1) /* 0xFF = unused byte */
/* ai_paint_cell */

/* "(x, z, value): paints a danger data cell (8 world units per cell), allocating a tile from the
   pool when needed" */
/* world_x, world_z: world position (inferred: same units as the 8 unit cell size); value: byte written into
   the cell's danger data. Returns 1 when the cell was painted, 0 when it is outside the grid or
   no free tile is left in the pool. Grid entries are 1-based tile numbers, 0 = no tile yet. */

undefined4 __cdecl ai_paint_cell(uint world_x,uint world_z,undefined4 value)

{
  short tile;
  int grid_index;
  int cell_x;

  /* cell_x = floor((world_x + 512) / 8), the sign fix-up makes negative values round down */
  cell_x = (int)(world_x + GRID_ORIGIN_OFFSET + ((int)(world_x + GRID_ORIGIN_OFFSET) >> 0x1f & 7U)) >> 3;
  /* grid_index = cell_z is computed in the same way; the cell must lie inside the 128 x 128 grid */
  if ((((-1 < cell_x) && (cell_x < GRID_CELLS)) &&
      (grid_index = (int)(world_z + GRID_ORIGIN_OFFSET + ((int)(world_z + GRID_ORIGIN_OFFSET) >> 0x1f & 7U)) >> 3, -1 < grid_index)) &&
     (grid_index < GRID_CELLS)) {
    /* from here grid_index is the row-major index into grid[] */
    grid_index = cell_x * GRID_CELLS + grid_index;
    tile = g_ai_data.grid[grid_index];
    /* cell already has a tile: skip the allocation and paint into it */
    if (0 < tile) {
LAB_0041a12b:
      /* paint: track the highest tile used, then write the byte at tile * 64 + local_z * 8 + local_x */
      cell_x = (int)tile;
      if (g_ai_data.tiles_used < cell_x) {
        g_ai_data.tiles_used = cell_x;
      }
      *(undefined1 *)((world_x & 7) + ((LONG_PTR)&g_ai_cells) + ((world_z & 7) + cell_x * 8) * 8) =
           (undefined1)value;
      return 1;
    }
    /* allocate: find the first tile in the pool whose 64 bytes are all TILE_EMPTY.
       cell_x is reused here as the byte index inside the candidate tile */
    for (tile = 1; tile < TILE_COUNT; tile = tile + 1) {
      cell_x = 0;
      while( true ) {
        /* every byte of the tile was empty: take it */
        if ((TILE_BYTES - 1) < cell_x) {
          /* not reachable with the loop bound above, kept from the original (inferred) */
          if ((TILE_COUNT - 1) < tile) {
            return 0;
          }
          g_ai_data.grid[grid_index] = tile;
          goto LAB_0041a12b;
        }
        /* tile holds data: try the next tile */
        if (*(char *)(cell_x + ((LONG_PTR)&g_ai_cells) + tile * TILE_BYTES) != TILE_EMPTY) break;
        cell_x = cell_x + 1;
      }
    }
  }
  return 0;
}


