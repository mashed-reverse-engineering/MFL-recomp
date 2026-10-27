#include "mfl_tier2.h"
/*00497070*/ extern int smplfzx_object_bounds_update();
/*005dd338*/ extern float k_f_0_01;
/*005dd9c4*/ extern float k_f_0_15;
/* object field offsets (from the obj pointer); bbox layout inferred from the copies below */
#define OBJ_BBOX_MIN_X (0x438)
#define OBJ_BBOX_MAX_X (0x444)
#define OBJ_FIELD_420 (0x420)
#define OBJ_FIELD_424 (0x424)
#define OBJ_FIELD_428 (0x428)
#define OBJ_FIELD_42C (0x42c)
#define OBJ_FIELD_430 (0x430)
#define OBJ_FIELD_434 (0x434)
#define OBJ_LIMIT_A (0x470) /* radius * 0.25, capped at 0.01 */
#define OBJ_LIMIT_B (0x46c) /* radius / 3, capped at 0.15 */
#define OBJ_RADIUS (0x468)
#define OBJ_FIELD_474 (0x474)
#define OBJ_EXTRA_VALUE (0x478) /* the extra_value argument; meaning unknown */
#define OBJ_ROW_FLAG_END (0x3a0) /* end of the per-row flag dwords */
#define OBJ_ROW_DWORD_END (0x340) /* end of the per-row dword table (16 dwords per row) */
#define OBJ_ROW_SHORT_END (0x120) /* end of the per-row short table (16 shorts per row) */
/* table sizes and sentinels */
#define ROW_COUNT (8)
#define ROW_CELLS (0x10)
#define SLOT_UNUSED (0xffff)
/* float bit patterns stored as integers: 0.5f, 0.01f, 0.15f */
#define FLOAT_0_5_BITS (0x3f000000)
#define FLOAT_0_01_BITS (0x3c23d70a)
#define FLOAT_0_15_BITS (0x3e19999a)
/* exit code passed when a pool header has a negative count (inferred) */
/* smplfzx_object_init_bounds */
/* "stores bbox, sets radius dependent limits (0.01/0.15 caps) and clears the cell tables" */
/* Sets up an smplfzx object: copies the bbox into the object (min xyz, then max xyz; inferred),
   stores radius and extra_value, and computes two radius dependent limits:
   OBJ_LIMIT_A = min(radius * 0.25, 0.01) and OBJ_LIMIT_B = min(radius / 3, 0.15).
   Then clears ROW_COUNT rows of cells (ROW_CELLS dwords and shorts each; inferred meaning).
   Returns obj. Note: the flag store in the row loop hits the fixed slot OBJ_ROW_FLAG_END every
   pass, not row_flag_cursor (kept as in the original, possibly a bug). */
int smplfzx_object_init_bounds(undefined4 *bbox,float radius,undefined4 extra_value,char *obj)
{
  float scaled;
  undefined4 *cell_dword;
  undefined2 *cell_short;
  int cells_left;
  undefined4 *row_dword_cursor;
  undefined2 *row_short_cursor;
  undefined4 *row_flag_cursor;
  int rows_left;

  *(undefined4 *)(obj + OBJ_EXTRA_VALUE) = extra_value;
  *(RwV3d *)(obj + OBJ_BBOX_MAX_X) = *(RwV3d *)(bbox + 3);
  *(RwV3d *)(obj + OBJ_BBOX_MIN_X) = *(RwV3d *)bbox;
  smplfzx_object_bounds_update(obj,FLOAT_0_5_BITS);
  scaled = radius * 0.25f;
  *(float *)(obj + OBJ_LIMIT_A) = scaled;
  if (scaled > k_f_0_01) {
    *(undefined4 *)(obj + OBJ_LIMIT_A) = FLOAT_0_01_BITS;
  }
  scaled = radius * 0.33333334f;
  *(float *)(obj + OBJ_LIMIT_B) = scaled;
  if (scaled > k_f_0_15) {
    *(undefined4 *)(obj + OBJ_LIMIT_B) = FLOAT_0_15_BITS;
  }
  *(float *)(obj + OBJ_RADIUS) = radius;
  *(undefined4 *)(obj + OBJ_FIELD_474) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_428) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_424) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_420) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_434) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_430) = 0;
  *(undefined4 *)(obj + OBJ_FIELD_42C) = 0;
  row_flag_cursor = (undefined4 *)(obj + OBJ_ROW_FLAG_END);
  row_dword_cursor = (undefined4 *)(obj + OBJ_ROW_DWORD_END);
  row_short_cursor = (undefined2 *)(obj + OBJ_ROW_SHORT_END);
  rows_left = ROW_COUNT;      /* diffemu: was `param_2 = 1.12104e-44` (the counter lived in the float parameter's stack slot) */
  /* one pass per row: clear the row's flag dword, step the dword and short row cursors down one row,
     then clear each cell of the row (dword to 0, short to SLOT_UNUSED) */
  do {
    row_flag_cursor = row_flag_cursor + -1;
    row_dword_cursor = row_dword_cursor + -ROW_CELLS;
    *row_flag_cursor = 0;
    row_short_cursor = row_short_cursor + -ROW_CELLS;
    *(undefined4 *)(obj + OBJ_ROW_FLAG_END) = 0;
    cells_left = ROW_CELLS;
    cell_short = row_short_cursor;
    cell_dword = row_dword_cursor;
    do {
      cell_dword = cell_dword + -1;
      cell_short = cell_short + -1;
      cells_left = cells_left + -1;
      *cell_dword = 0;
      *cell_short = SLOT_UNUSED;
    } while (cells_left != 0);
    rows_left = rows_left + -1;
  } while (rows_left != 0);
  return obj;
}
