#include "mfl_tier2.h"
/*0045d590*/ extern int course_zone_contains_matrix_in_zone_6a2888();
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*006a9728*/ extern undefined4 g_weapon_6a9728;
/*006aab78*/ extern char g_weapon_6aab78[];
/* Side length of the square grid of entries walked per pass (inferred: 5 rows x 5 columns). */
#define GRID_DIM (5)
/*
 * weapon_flamethrower_course_33_update (inferred: flamethrower course-33 update)
 *
 * Walks the entries from g_weapon_6a9728 up to g_weapon_6aab78, GRID_DIM x GRID_DIM at a time.
 * For each entry with translation[4].x non-zero and translation[2].x < 1.0 (inferred: active
 * flag and a range value), the entry's position goes into a matrix, is passed through
 * course_zone_contains_matrix_in_zone_6a2888 (which may move it back into the zone), and the
 * result is written back to the entry's x/y/z. Takes no parameters, returns nothing.
 */
void weapon_flamethrower_course_33_update(void)
{
  int row_left;
  RwV3d *translation;
  int col_left;
  unsigned int flags;
  RwMatrix matrix;

  translation = (RwV3d *)&g_weapon_6a9728;
  /* Outer loop: repeats over the table until translation reaches g_weapon_6aab78. */
  do {
    /* Row loop: GRID_DIM rows per pass. */
    row_left = GRID_DIM;
    do {
      /* Column loop: GRID_DIM entries per row; each step advances 52 bytes (13 floats, inferred). */
      col_left = GRID_DIM;
      do {
        /* Only entries with translation[4].x != 0 (inferred: active) and translation[2].x < 1.0 (inferred: in range). */
        if ((*(int *)&translation[4].x != 0) && (translation[2].x < float_1_0)) {
          flags = matrix.flags | 0x20003;
          matrix.at.z = 1.0;
          matrix.up.y = 1.0;
          matrix.right.x = 1.0;
          matrix.up.x = 0.0;
          matrix.right.z = 0.0;
          matrix.right.y = 0.0;
          matrix.at.y = 0.0;
          matrix.at.x = 0.0;
          matrix.up.z = 0.0;
          matrix.pos.z = 0.0;
          matrix.pos.y = 0.0;
          matrix.pos.x = 0.0;
          matrix.flags = flags;
          RwMatrixTranslate(&matrix,translation,rwCOMBINEREPLACE);
          course_zone_contains_matrix_in_zone_6a2888(&matrix,&matrix);
          *translation = matrix.pos;
        }
        translation = (RwV3d *)&translation[4].y;
        col_left = col_left + -1;
      } while (col_left != 0);
      row_left = row_left + -1;
    } while (row_left != 0);
  } while ((LONG_PTR)translation < ((LONG_PTR)&g_weapon_6aab78));
  return;
}
