#include "mfl_tier2.h"
/*0044c550*/ extern char *camera_get_8c7470();
/*004757a0*/ extern char *get_viewport_camera_matrix();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd414*/ extern float k_f_4_5;
/*005ddc28*/ extern float k_f_0;
/*005df448*/ extern float k_f_18;
/*005e06e0*/ extern float k_f_2_25_5e06e0;
/*005e06e4*/ extern float k_f_0_93;
/*00703890*/ extern undefined4 g_trigger_sphere_703890[0x1000];
/*007106d8*/ extern char *g_trigger_sphere_7106d8;
/*0071aafc*/ extern undefined4 g_particles_71aafc;

#define GRID_HALF_CELLS (5)
#define GRID_STOP_INDEX (-6)
#define CELL_STRIDE_WORDS (0x6e)
#define ROW_STRIDE_WORDS (0x4ba)
#define MATRIX_AT_X (0x20)
#define MATRIX_AT_Y (0x24)
#define MATRIX_AT_Z (0x28)
/* trigger_grid_build_visible_cells */

/* Rebuilds the trigger grid for the current view: an 11x11 grid of cells (offsets -5..5 in
   GRID_HALF_CELLS steps of k_f_4_5) is written into g_trigger_sphere_703890, one record of
   CELL_STRIDE_WORDS words per cell, rows ROW_STRIDE_WORDS apart.
   A cell's direction is the view vector (RwMatrix at, MATRIX_AT_*) times k_f_18, plus the cell's
   grid offset, normalised. The cell is kept when that direction's dot product with the view vector
   is at least k_f_0_93 (0.93, the cone cutoff). Kept cells get an x/z box of k_f_2_25_5e06e0 (2.25,
   inferred half size) around the camera position plus the cell offset, and cull_flag = 0; the
   others get cull_flag = 1. Takes no parameters, returns nothing.
   Record layout (word offset from cell): -7 max_x, -6 base_y, -5 max_z, -4 min_x, -3 center_y,
   -2 min_z, -8/-1/0 cleared when kept, +1 cull_flag.
   (was: "places a 5x5 grid of cells around the camera, keeps the cells whose direction is within
   0.93 dot of the view" - the grid is 11x11) */

void trigger_grid_build_visible_cells(void)

{
  float tmp_x;
  float cam_z;
  float row_x;
  float half_extent;
  char *view_matrix;
  float *cam_pos;
  float *min_x;
  float *center_y;
  undefined4 *row_cells;
  undefined4 *cull_flag;
  struct {
    int col;
    float *min_z;
    float *max_x;
    float *max_z;
    undefined4 *flag_m8;
    undefined4 *cell;
    undefined4 *flag_m1;
    float *base_y;
    int row;
    undefined1 pad_20[8];
    RwV3d dir;
    float cell_x;
    undefined1 pad_8[4];
    float cell_z;
  } scan;

  view_matrix = get_viewport_camera_matrix(0);
  cam_pos = (float *)camera_get_8c7470();
  row_cells = g_trigger_sphere_703890;
  scan.row = GRID_HALF_CELLS;
  /* Cache the camera position pointer in the global; the normalise branch reloads it from there. */
  g_trigger_sphere_7106d8 = (char *)cam_pos;
  /* Outer loop: one pass per grid row (row = 5 down to -5, along x). */
  do {
    scan.flag_m1 = row_cells + -1;
    scan.flag_m8 = row_cells + -8;
    row_x = (float)scan.row * k_f_4_5;
    scan.max_z = (float *)(row_cells + -5);
    scan.base_y = (float *)(row_cells + -6);
    scan.max_x = (float *)(row_cells + -7);
    scan.min_z = (float *)(row_cells + -2);
    scan.col = GRID_HALF_CELLS;
    center_y = (float *)(row_cells + -3);
    min_x = (float *)(row_cells + -4);
    cull_flag = row_cells + 1;
    scan.cell = row_cells;
    /* Inner loop: one pass per column (col = 5 down to -5, along z), one cell record each. */
    do {
      scan.cell_z =(float)scan.col * k_f_4_5;
      scan.dir.x = *(float *)(view_matrix + MATRIX_AT_X) * k_f_18 + row_x;
      scan.dir.y = *(float *)(view_matrix + MATRIX_AT_Y) * k_f_18;
      scan.dir.z = *(float *)(view_matrix + MATRIX_AT_Z) * k_f_18 + scan.cell_z;
      scan.cell_x = row_x;
      if (((scan.dir.x != k_f_0) || (scan.dir.y != k_f_0)) || (scan.dir.z != k_f_0)) {
        RwV3dNormalize(&scan.dir,&scan.dir);
        cam_pos = (float *)g_trigger_sphere_7106d8;
      }
      /* Assume visible; the cone test below clears this to 0 or sets it to 1. */
      *cull_flag = 0;
      /* Cone test: dot(normalised direction, view) >= 0.93 means the cell is inside the view cone. */
      if (k_f_0_93 <=
          scan.dir.z * *(float *)(view_matrix + MATRIX_AT_Z) +
          scan.dir.x * *(float *)(view_matrix + MATRIX_AT_X) +
          scan.dir.y * *(float *)(view_matrix + MATRIX_AT_Y)) {
        tmp_x = *cam_pos;
        cam_z = cam_pos[2];
        *min_x = (scan.cell_x + tmp_x) - k_f_2_25_5e06e0;
        half_extent = k_f_2_25_5e06e0;
        *center_y = cam_pos[1];
        *scan.min_z = (scan.cell_z + cam_z) - half_extent;
        *scan.max_x = k_f_2_25_5e06e0 + scan.cell_x + tmp_x;
        tmp_x = k_f_2_25_5e06e0;
        *scan.base_y = cam_pos[1];
        *scan.max_z = tmp_x + scan.cell_z + cam_z;
        *scan.flag_m8 = 0;
        *scan.cell = 0;
        *scan.flag_m1 = 0;
      }
      else {
        /* Outside the view cone: cull the cell and leave its box untouched. */
        *cull_flag = 1;
      }
      scan.base_y = scan.base_y + CELL_STRIDE_WORDS;
      scan.flag_m1 = scan.flag_m1 + CELL_STRIDE_WORDS;
      scan.cell = scan.cell + CELL_STRIDE_WORDS;
      scan.flag_m8 = scan.flag_m8 + CELL_STRIDE_WORDS;
      scan.max_z = scan.max_z + CELL_STRIDE_WORDS;
      scan.max_x = scan.max_x + CELL_STRIDE_WORDS;
      scan.min_z = scan.min_z + CELL_STRIDE_WORDS;
      cull_flag = cull_flag + CELL_STRIDE_WORDS;
      center_y = center_y + CELL_STRIDE_WORDS;
      min_x = min_x + CELL_STRIDE_WORDS;
      scan.col = scan.col + -1;
    } while (GRID_STOP_INDEX < scan.col);
    scan.row = scan.row + -1;
    row_cells = row_cells + ROW_STRIDE_WORDS;
  } while (GRID_STOP_INDEX < scan.row);
  /* (inferred) marks the trigger grid as rebuilt for this frame. */
  g_particles_71aafc = 1;
  return;
}


