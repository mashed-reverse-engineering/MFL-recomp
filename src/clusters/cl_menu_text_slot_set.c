// cluster-entries: menu_text_slot_set
/* Installed cluster menu_text_slot_set (tools/cluster_install.py from output/cluster_pilot10/menu_text_slot_set.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/* menu_text_slot_set */

/* WARNING: Unknown calling convention */
/* "fills a 0x34 byte text slot: id, colour white, scale 0.6, position" */

void menu_text_slot_set_n(int *in_eax, int in_edx, int param_3, int param_4, int param_5)

{
  g_menu_item_anims[*in_eax].item_id = in_edx;
  *(undefined1 *)&g_menu_item_anims[*in_eax].colour = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*in_eax].colour + 1) = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*in_eax].colour + 2) = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*in_eax].colour + 3) = 0xff;
  g_menu_item_anims[*in_eax].scale = 0.6;
  g_menu_item_anims[*in_eax].x = (float)param_3;
  g_menu_item_anims[*in_eax].y = (float)param_4;
  g_menu_item_anims[*in_eax].param = param_5;
  return;
}

/* entry thunk at the original address of menu_text_slot_set: register inputs (eax,edx,s0,s1,s2) -> natural C menu_text_slot_set_n; ret 0 */
__declspec(naked) void menu_text_slot_set(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push edx
        push eax
        call menu_text_slot_set_n
        add esp, 20
        ret
    }
}
