#include "mfl_tier2.h"
/*004311b0*/ extern int message_show(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*004316f0*/ extern int message_close();
/*0069cfd0*/ extern undefined4 g_message_state[0x1000];
/*0069cfd4*/ extern undefined4 g_message_text_id;
/*0069cff0*/ extern undefined4 g_message_code;
/*0069cff4*/ extern undefined4 g_message_buttons;
/*0069d228*/ extern undefined4 g_menu_69d228[0x1000];
/*0069d22c*/ extern undefined4 g_menu_69d22c;
/*0069d248*/ extern undefined4 g_message_69d248;
/*0069d24c*/ extern undefined4 g_menu_69d24c;
#define MESSAGE_STATE_WORDS (0x11) /* size of the message state block in dwords */
/* message_replace_if_changed */
/* "if the message differs from the shown one copies the new state, closes and shows it (tail call: message_show gets the caller's six arguments unchanged)" */
void __cdecl message_replace_if_changed(undefined4 show_arg_1,undefined4 show_arg_2,undefined4 show_arg_3,undefined4 show_arg_4, undefined4 show_arg_5,undefined4 show_arg_6)
{
  int words_left;
  undefined4 *src;
  undefined4 *dst;
  
  if (((g_message_69d248 == g_message_code) && (g_menu_69d24c == g_message_buttons)) &&
     (g_menu_69d22c == g_message_text_id)) {
    return;
  }
  src = g_message_state;
  dst = g_menu_69d228;
  for (words_left = MESSAGE_STATE_WORDS; words_left != 0; words_left = words_left + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  message_close();
  message_show(show_arg_1,show_arg_2,show_arg_3,show_arg_4,show_arg_5,show_arg_6);
  return;
}
