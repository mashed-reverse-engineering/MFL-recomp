#include "mfl_tier2.h"
/*005dd410*/ extern float k_f_n2;
/*005ddb5c*/ extern float k_f_n4;
/*008be720*/ extern undefined4 g_weapon_8be720;
/*008be7b0*/ extern char g_sound_frame_8be7b0[];

#define WEAPON_VALUES_PER_ROW (6)

/* sound_ducking_values_decay */

/* "steps each float of g_weapon_8be720 (6 per row) toward 0 through -1 and -2" */

/*
 * Decays the per-weapon sound ducking values in g_weapon_8be720 one step toward 0.
 * Takes no parameters and returns nothing; it updates the table in place.
 * Walks the table row by row (WEAPON_VALUES_PER_ROW floats per row) until value_ptr
 * reaches g_sound_frame_8be7b0, which is taken as the end of the table (inferred).
 * Per value: above k_f_n2 -> 0.0; above k_f_n4 up to k_f_n2 -> -1.0; at or below k_f_n4 -> -2.0.
 * The values of k_f_n2 and k_f_n4 are not in the dossier (inferred: the -1 and -2 levels).
 */
void sound_ducking_values_decay(void)

{
  float *value_ptr;
  int values_left_in_row;

  value_ptr = (float *)&g_weapon_8be720;
  do {
    /* one row: count down its values */
    values_left_in_row = WEAPON_VALUES_PER_ROW;
    do {
      /* above k_f_n2 -> 0.0; above k_f_n4 -> -1.0; otherwise -2.0 */
      if (*value_ptr > k_f_n2) {
        *value_ptr = 0.0;
      }
      else if ((*value_ptr <= k_f_n2) && (*value_ptr > k_f_n4)) {
        *value_ptr = -1.0;
      }
      else if (*value_ptr <= k_f_n4) {
        *value_ptr = -2.0;
      }
      value_ptr = value_ptr + 1;
      values_left_in_row = values_left_in_row + -1;
    } while (values_left_in_row != 0);
  } while ((LONG_PTR)value_ptr < ((LONG_PTR)&g_sound_frame_8be7b0));
  return;
}


