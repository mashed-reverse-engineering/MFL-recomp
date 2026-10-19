#include "mfl_tier2.h"
/*0076ed18*/ extern undefined4 g_app_76ed18;
/*0076efe8*/ extern char *g_joypad_get_76efe8;
/*0076efec*/ extern char *g_joypad_get_76efec;
/*0076f068*/ extern char *g_joypad_get_76f068;
/*0076f06c*/ extern char *g_joypad_get_76f06c;
/*0076f128*/ extern char *g_joypad_get_76f128;
/*00770f64*/ extern undefined4 g_app_770f64;
#define JOYPAD_AXIS_COUNT (0x89) /* axis records per joypad (tables 76efe8 / 76efec / 76f068 / 76f06c) */
#define AXIS_WORDS_PER_JOYPAD (0x112) /* 4-byte words per joypad in table 76f128: 2 * JOYPAD_AXIS_COUNT */
/* joypad_get_axis_info */

/* "(joypad, axis, out): axis record 0x89 entries x 8 bytes per joypad" */

/*
 * Copies the stored axis data for one joypad axis into up to three caller buffers.
 *   joypad, axis : indices, checked against g_app_770f64 (joypad count) and g_app_76ed18 (axes per joypad) (inferred)
 *   out_a        : 8-byte record from tables 76efe8/76efec (inferred: two 32-bit words)
 *   out_b        : 8-byte record from tables 76f068/76f06c (inferred: two 32-bit words)
 *   out_c        : one 32-bit word from table 76f128 (inferred)
 * Any out pointer may be NULL to skip that copy.
 * Returns 1 if the indices were valid (data copied), 0 otherwise.
 */
undefined4 __cdecl joypad_get_axis_info(int joypad,int axis,undefined4 *out_a,undefined4 *out_b,undefined4 *out_c)

{
  int record_index;

  if ((joypad < (int)g_app_770f64) && (axis < (int)g_app_76ed18)) {
    /* first record pair, if requested */
    if (out_a != (undefined4 *)0x0) {
      record_index = joypad * JOYPAD_AXIS_COUNT + axis;
      *out_a = *(undefined4 *)((char *)&g_joypad_get_76efe8 + record_index * 8);
      out_a[1] = *(undefined4 *)((char *)&g_joypad_get_76efec + record_index * 8);
    }
    /* second record pair, if requested */
    if (out_b != (undefined4 *)0x0) {
      record_index = joypad * JOYPAD_AXIS_COUNT + axis;
      *out_b =*(undefined4 *)((char *)&g_joypad_get_76f068 + record_index * 8);
      out_b[1] = *(undefined4 *)((char *)&g_joypad_get_76f06c + record_index * 8);
    }
    /* single word, indexed with the 4-byte stride of table 76f128, if requested */
    if (out_c != (undefined4 *)0x0) {
      *out_c = *(undefined4 *)((char *)&g_joypad_get_76f128 + (joypad * AXIS_WORDS_PER_JOYPAD + axis) * 4);
    }
    return 1;
  }
  return 0;
}


