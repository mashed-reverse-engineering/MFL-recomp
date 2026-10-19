#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*005dda20*/ extern float k_f_4_2949673e9;
extern float func_0x004757e0();
extern void __cdecl func_0x0045bad0(float f);
extern void __cdecl func_0x00449c40(int m, float f);
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
  float10 value_f;

  /* Unsigned-to-float conversion: negative raw values are shifted up by 2^32. */
  value_f = (float10)raw_value;
  if (raw_value < 0) {
    value_f = value_f + (float10)k_f_4_2949673e9;
  }
  func_0x0045bad0((float)value_f);
  /* The camera float is the second argument of func_0x00449c40 (Ghidra attached it to get_viewport_camera_matrix). */
  func_0x00449c40(get_viewport_camera_matrix(0),func_0x004757e0(0));
  return;
}



