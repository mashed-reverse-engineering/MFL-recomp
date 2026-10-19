#include "mfl_tier2.h"
/*006c2884*/ extern undefined4 g_car_6c2884;
/*006c28b0*/ extern undefined4 g_car_6c28b0;
/*006c28b4*/ extern undefined4 g_car_6c28b4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define CELL_PTR_SIZE (4) /* sizeof a cell pointer, the index stride in bytes */
#define GRID_STATE_WORDS (0xf) /* words after g_car_6c2884 that are reset to 0 (inferred) */
/* course_grid_buffers_free */

/* "frees every cell buffer of the grid" */

/*
 * Frees every cell buffer of the grid, then the grid's pointer array, and zeroes
 * the grid state words. Takes no parameters and returns nothing.
 * The cells are pointers in an array at g_car_6c2884, indexed stride * cell_y + cell_x.
 * (Meaning of the g_car_* globals inferred from the indexing: cell_x runs to
 * g_car_6c28b0, cell_y and stride come from g_car_6c28b4.)
 */

void course_grid_buffers_free(void)

{
  void *cell_buffer;
  int stride;     /* copy of g_car_6c28b4, reused as the countdown in the last loop */
  int cell_y;
  int cell_x;
  undefined4 *clear_ptr;

  cell_x = 0;
  stride = g_car_6c28b4;
  /* for each cell_x up to g_car_6c28b0: walk all cell_y rows of that column */
  if (0 < (int)g_car_6c28b0) {
    do {
      cell_y = 0;
      if (0 < stride) {
        do {
          /* free the cell buffer if it is set, then clear its slot */
          cell_buffer = *(void **)((int)g_car_6c2884 + (stride * cell_y + cell_x) * CELL_PTR_SIZE);
          if (cell_buffer != (void *)0x0) {
            RwFree(cell_buffer);
            *(undefined4 *)((int)g_car_6c2884 + (g_car_6c28b4 * cell_y + cell_x) * CELL_PTR_SIZE) = 0;
            stride = g_car_6c28b4;
          }
          cell_y = cell_y + 1;
        } while (cell_y < stride);
      }
      cell_x = cell_x + 1;
    } while (cell_x < (int)g_car_6c28b0);
  }
  /* free the pointer array itself, then zero GRID_STATE_WORDS words starting at g_car_6c2884 */
  RwFree(g_car_6c2884);
  clear_ptr = &g_car_6c2884;
  for (stride = GRID_STATE_WORDS; stride != 0; stride = stride + -1) {
    *clear_ptr = 0;
    clear_ptr = clear_ptr + 1;
  }
  return;
}


