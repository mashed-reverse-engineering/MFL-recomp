#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*005dda20*/ extern float k_f_4_2949673e9;
extern float func_0x004757e0();
/* course_sandsnow_cb1 */

/* WARNING: Unknown calling convention */

/* Course callback for the Sands & Snow track (cb1, inferred from name).
   unused_context: not read by this function.
   raw_value: 32-bit value treated as unsigned (inferred); converted to float, with 2^32 (k_f_4_2949673e9) added when negative.
   Passes the converted value to func_0x0045bad0, then sets the viewport camera matrix from
   func_0x004757e0(0) (inferred: a float value) and submits the matrix via func_0x00449c40.
   Returns nothing. */
void course_sandsnow_cb1(char *unused_context,int raw_value)

{
  float camera_value_bits;
  float value_f;
  undefined4 camera_matrix;
  float10 camera_value;

  /* Unsigned-to-float conversion: negative raw values are shifted up by 2^32. */
  value_f = (float)raw_value;
  if (raw_value < 0) {
    value_f = value_f + k_f_4_2949673e9;
  }
  func_0x0045bad0(value_f);
  /* Camera float is passed to get_viewport_camera_matrix as its raw bit pattern. */
  camera_value = (float10)func_0x004757e0(0);
  camera_matrix = get_viewport_camera_matrix(0,(camera_value_bits = (float)camera_value, *(int *)&camera_value_bits));
  func_0x00449c40(camera_matrix);
  return;
}



