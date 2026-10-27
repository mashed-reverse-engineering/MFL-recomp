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
/* message_replace_if_changed */
/* "if the message differs from the shown one copies the new state, closes and shows it (tail call: message_show gets the caller's six arguments unchanged)" */
void __cdecl message_replace_if_changed(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, undefined4 param_5,undefined4 param_6)
{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  if (((g_message_69d248 == g_message_code) && (g_menu_69d24c == g_message_buttons)) &&
     (g_menu_69d22c == g_message_text_id)) {
    return;
  }
  puVar2 = g_message_state;
  puVar3 = g_menu_69d228;
  for (iVar1 = 0x11; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  message_close();
  message_show(param_1,param_2,param_3,param_4,param_5,param_6);
  return;
}
