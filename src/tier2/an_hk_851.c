#include "mfl_tier2.h"
/*005dd374*/ extern float k_f_0_2;
/*005dd378*/ extern float k_f_0_1;
/*005dd408*/ extern float k_f_n5;
/*005dd41c*/ extern float float_5_0;
/*005dd9c4*/ extern float k_f_0_15;
/*005ddc28*/ extern float k_f_0;
/*005de230*/ extern float k_f_n0_75;
/*005de270*/ extern float k_f_n0_1;
/*005de480*/ extern float k_f_n0_4;
/*005df7b8*/ extern float k_f_n0_2;
/*005df938*/ extern float k_f_n1_75;
/*008be720*/ extern undefined4 g_weapon_8be720;
#define DUCK_MATRIX_SIZE (6)
#define DUCK_TABLE_LAST_BYTE (0x8f)
#define DUCK_LIMIT (5.0)
#define DUCK_ANY_PARTNER (-1)
/* sound_ducking_adjust_by_event */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "adds per voice class offsets (by id 3, 0xA, 0xF, 0x13, 0x17, 0x1A) to the g_weapon_8be720 table"
    */

/* Adjusts the sound ducking matrix g_weapon_8be720 for one voice class event (inferred).
   The matrix is 6x6 floats, row-major: cell [row][col] is at row * 6 + col. The Ghidra name is a
   weapon name, the data is a matrix.
   voice_class: picks the offsets below; ids are not in the known enum list, so they stay numeric.
   class_index: the class whose cells are changed (inferred).
   partner_index: the class paired with it, or DUCK_ANY_PARTNER (-1) for "class_index column only, all rows".
   Adds the offsets to the cells, then clamps every cell to [-DUCK_LIMIT, DUCK_LIMIT]. Returns nothing. */

void sound_ducking_adjust_by_event(undefined4 voice_class,int class_index,int partner_index)

{
  int cell_index;
  float primary_offset;
  float pair_offset;
  float pending_offset;
  float partner_offset;
  float column_offset;
  float *cell_ptr;
  int row_base;
  int row;
  int col;
  bool skip_column;

  /* Offsets per voice class (inferred meanings):
     primary_offset: cell [partner][class]
     pair_offset:    cell [class][partner]
     partner_offset: cell [row][partner] for rows other than class
     column_offset:  cell [row][class] for rows other than partner
     Zero for any voice class not listed in the switch. */
  primary_offset = k_f_0;
  pair_offset = k_f_0;
  partner_offset = k_f_0;
  column_offset = k_f_0;
  switch(voice_class) {
  case 3:
    primary_offset = k_f_n1_75;
    column_offset = k_f_0_2;
    break;
  default:
    break;
  case 10:
  case 0x17:
    primary_offset = k_f_n0_2;
    break;
  case 0xf:
    primary_offset = k_f_n0_75;
    column_offset = k_f_0_15;
    break;
  case 0x13:
    primary_offset = k_f_n0_4;
    pair_offset = k_f_n0_4;
    partner_offset = k_f_0_1;
    column_offset = k_f_0_1;
    break;
  case 0x1a:
    column_offset = k_f_n0_1;
  }
  row = 0;
  row_base = 0;
  cell_ptr = (float *)&g_weapon_8be720;
  /* Walk the matrix row by row; row_base is row * 6 and cell_ptr the address of the current cell. */
  do {
    col = 0;
    /* Walk the 6 columns of this row. */
    do {
      if (partner_index == DUCK_ANY_PARTNER) {
        /* Any partner: skip_column stays false (row_base == -6 never happens), so only the column update runs. */
        skip_column = row_base == -6;
      }
      else {
        /* Cell [partner][class] += primary_offset. */
        if ((row == partner_index) && (col == class_index)) {
          cell_index = class_index + partner_index * DUCK_MATRIX_SIZE;
          ((float *)&g_weapon_8be720)[cell_index] = primary_offset + ((float *)&g_weapon_8be720)[cell_index];
        }
        /* Row == class: cell [class][partner] += pair_offset.
           Other rows: cell [row][partner] += partner_offset. Both paths reach the add at LAB_004688de. */
        if (row == class_index) {
          if (col == partner_index) {
            cell_index = class_index * DUCK_MATRIX_SIZE;
            pending_offset = pair_offset;
LAB_004688de:
            ((float *)&g_weapon_8be720)[partner_index + cell_index] = pending_offset + ((float *)&g_weapon_8be720)[partner_index + cell_index];
          }
        }
        else {
          cell_index = row_base;
          pending_offset = partner_offset;
          if (col == partner_index) goto LAB_004688de;
        }
        skip_column = row == partner_index;
      }
      /* Column class_index in every row except the partner row (every row when partner is DUCK_ANY_PARTNER):
         cell [row][class] += column_offset. */
      if ((!skip_column) && (col == class_index)) {
        ((float *)&g_weapon_8be720)[row_base + class_index] = column_offset + ((float *)&g_weapon_8be720)[row_base + class_index];
      }
      /* Clamp: below k_f_n5 becomes -DUCK_LIMIT, above float_5_0 becomes DUCK_LIMIT (inferred bounds -5 / +5). */
      if (!(k_f_n5 > *cell_ptr)) {
        if (float_5_0 < *cell_ptr) {
          *cell_ptr = DUCK_LIMIT;
        }
      }
      else {
        *cell_ptr = -DUCK_LIMIT;
      }
      col = col + 1;
      cell_ptr = cell_ptr + 1;
    } while (col < DUCK_MATRIX_SIZE);
    row = row + 1;
    row_base = row_base + DUCK_MATRIX_SIZE;
    /* Stop after the last of the 36 cells (last byte 0x8f = 36 * 4 - 1). */
    if ((char *)&g_weapon_8be720 + DUCK_TABLE_LAST_BYTE < (char *)cell_ptr) {
      return;
    }
  } while( true );
}


