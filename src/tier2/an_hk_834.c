#include "mfl_tier2.h"
/*004a48e0*/ extern int app_get_app_770f64();
/*004a48f0*/ extern int __cdecl joypad_get_axis_info(undefined4 p0, undefined4 p1, void *p2, undefined4 p3, void *p4);
/*004a5250*/ extern int __cdecl keyboard_state_to_bits(void *p0, void *p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*007711c4*/ extern undefined g_app_7711c4[0x1000];
/*0077120c*/ extern undefined g_app_77120c[0x1000];
/*0077122c*/ extern undefined g_app_77122c;

#define JOYPAD_AXIS_STRIDE (8)   /* bytes per joypad in g_app_77120c */
#define JOYPAD_WORD_STRIDE (4)   /* bytes per joypad in g_app_7711c4 */
#define KEYBOARD_BITS_SIZE (0x20) /* bytes in the keyboard buffer g_app_77122c (inferred) */

/* joypads_poll_axes_and_keyboard */

/* "polls all joypad axes and the keyboard bits"
   Polls every joypad slot (count from app_get_app_770f64) into the axis block g_app_77120c
   and the word block g_app_7711c4 (inferred: 4-byte status per joypad), then clears the
   keyboard buffer and refills it with keyboard_state_to_bits. No parameters, no return value. */

void joypads_poll_axes_and_keyboard(void)

{
  int joypad_count;
  undefined *out_ptr;
  int joypad_index;
  undefined *joypad_word_out;
  undefined4 keyboard_bits_size;

  /* one joypad per iteration: the axis and word cursors advance by their strides */
  joypad_count = app_get_app_770f64();
  joypad_index = 0;
  if (0 < joypad_count) {
    out_ptr = g_app_77120c;
    joypad_word_out = g_app_7711c4;
    do {
      joypad_get_axis_info(joypad_index,0,out_ptr,0,joypad_word_out);
      joypad_index = joypad_index + 1;
      joypad_word_out = joypad_word_out + JOYPAD_WORD_STRIDE;
      out_ptr = out_ptr + JOYPAD_AXIS_STRIDE;
    } while (joypad_index < joypad_count);
  }
  /* keyboard: zero the buffer, then fill it (out_ptr is reused here as the keyboard buffer pointer) */
  keyboard_bits_size = KEYBOARD_BITS_SIZE;
  out_ptr = &g_app_77122c;
  mem_zero((void *)(((char *)&g_app_77122c)), KEYBOARD_BITS_SIZE);
  keyboard_state_to_bits(&g_app_77122c,out_ptr,keyboard_bits_size);
  return;
}


