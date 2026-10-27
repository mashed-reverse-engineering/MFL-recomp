// cluster-entries: menu_text_slot_set
/* Installed cluster menu_text_slot_set (tools/cluster_install.py from output/cluster_pilot10/menu_text_slot_set.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/* menu_text_slot_set */

/* WARNING: Unknown calling convention */
/* "fills a 0x34 byte text slot: id, colour white, scale 0.6, position" */

void menu_text_slot_set_n(int *slot_index_ptr, int item_id, int x, int y, int item_param)

{
  g_menu_item_anims[*slot_index_ptr].item_id = item_id;
  *(undefined1 *)&g_menu_item_anims[*slot_index_ptr].colour = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*slot_index_ptr].colour + 1) = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*slot_index_ptr].colour + 2) = 0xff;
  *(undefined1 *)((int)&g_menu_item_anims[*slot_index_ptr].colour + 3) = 0xff;
  g_menu_item_anims[*slot_index_ptr].scale = 0.6;
  g_menu_item_anims[*slot_index_ptr].x = (float)x;
  g_menu_item_anims[*slot_index_ptr].y = (float)y;
  g_menu_item_anims[*slot_index_ptr].param = item_param;
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
