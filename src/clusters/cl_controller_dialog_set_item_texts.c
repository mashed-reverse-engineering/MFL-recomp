// cluster-entries: controller_dialog_set_item_texts
/* Installed cluster controller_dialog_set_item_texts (tools/cluster_install.py from output/cluster_pilot_zy/controller_dialog_set_item_texts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/*0076e874*/ extern int g_language_offset;
/*007711a4*/ extern char g_controller_dialog_7711a4[];
/*007711e8*/ extern char g_controller_dialog_7711e8[];
/*004a93b0*/ extern void dialog_item_set_text(void *hwnd, int text);
extern __declspec(dllimport) void *__stdcall GetDlgItem(void *hwnd, int id);
extern __declspec(dllimport) int __stdcall SendMessageA(void *hwnd, unsigned int msg, unsigned int wparam, int lparam);

/* Fills in the controller dialog `dialog`: caption and button texts from the language table, then for each of the 4 player combo boxes the name of its controller
   (gamepad: its name, keyboard: "Keyboard") plus the "None" entry (CB_ADDSTRING / CB_SETITEMDATA 1 / 2 / 0) and selects entry 0. */
void controller_dialog_set_item_texts_n(void *dialog)
{
  void *combo[4];
  void *item;
  unsigned int index;
  int i;

  dialog_item_set_text(dialog, g_language_offset + 0x7ee);
  dialog_item_set_text(GetDlgItem(dialog, 0x411), g_language_offset + 0x7f8);
  dialog_item_set_text(GetDlgItem(dialog, 0x412), g_language_offset + 0x802);
  dialog_item_set_text(GetDlgItem(dialog, 0x413), g_language_offset + 0x80c);
  dialog_item_set_text(GetDlgItem(dialog, 0x414), g_language_offset + 0x816);
  dialog_item_set_text(GetDlgItem(dialog, 0x3ef), g_language_offset + 0x820);
  dialog_item_set_text(GetDlgItem(dialog, 0x3f0), g_language_offset + 0x820);
  dialog_item_set_text(GetDlgItem(dialog, 0x3f1), g_language_offset + 0x820);
  dialog_item_set_text(GetDlgItem(dialog, 0x3f2), g_language_offset + 0x820);
  dialog_item_set_text(GetDlgItem(dialog, 1), g_language_offset + 2000);
  combo[0] = GetDlgItem(dialog, 0x3f3);
  combo[1] = GetDlgItem(dialog, 0x3f4);
  combo[2] = GetDlgItem(dialog, 0x3ea);
  combo[3] = GetDlgItem(dialog, 0x3eb);
  for (i = 0; i < 4; i++) {
    item = combo[i];
    if (g_controller_cfg[i].device == 1) {
      index = SendMessageA(item, 0x143, 0, (int)g_controller_cfg[i].name);
      SendMessageA(item, 0x151, index, 1);
    } else if (g_controller_cfg[i].device == 2) {
      index = SendMessageA(item, 0x143, 0, (int)g_controller_dialog_7711a4);
      SendMessageA(item, 0x151, index, 2);
    }
    item = combo[i];
    index = SendMessageA(item, 0x143, 0, (int)g_controller_dialog_7711e8);
    SendMessageA(item, 0x151, index, 0);
    SendMessageA(item, 0x14e, 0, 0);
  }
}

/* entry thunk at the original address of controller_dialog_set_item_texts: register inputs (eax) -> natural C controller_dialog_set_item_texts_n; ret 0 */
__declspec(naked) void controller_dialog_set_item_texts(void)
{
    __asm {
        push eax
        call controller_dialog_set_item_texts_n
        add esp, 4
        ret
    }
}
