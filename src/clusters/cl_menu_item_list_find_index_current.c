// cluster-entries: menu_item_list_find_index_current
/* Installed cluster menu_item_list_find_index_current (tools/cluster_install.py from output/cluster_pilot11/menu_item_list_find_index_current.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/* menu_item_list_find_index_current */

/* "index of the id in EBX in the item list of the current menu": returns the list entry that follows the occurrence-th occurrence of item_id (not a position), -1 when the terminator is reached first */

int menu_item_list_find_index_current_n(int item_id, int occurrence)

{
  int *list;
  int entry;
  int pos;
  int match_count;
  
  list = (int *)g_menu_records[g_menu_current].item_list;
  entry = *list;
  match_count = 0;
  pos = 0;
  do {
    if (entry == -0xf90000) {
      return -1;
    }
    if (entry == item_id) {
      if (occurrence == match_count) {
        return list[pos + 1];
      }
      match_count = match_count + 1;
    }
    entry = list[pos + 1];
    pos = pos + 1;
  } while( true );
}

/* entry thunk at the original address of menu_item_list_find_index_current: register inputs (ebx,edi) -> natural C menu_item_list_find_index_current_n; ret 0 */
__declspec(naked) void menu_item_list_find_index_current(void)
{
    __asm {
        push edi
        push ebx
        call menu_item_list_find_index_current_n
        add esp, 8
        ret
    }
}
