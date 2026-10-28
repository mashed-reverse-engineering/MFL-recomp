// cluster-entries: menu_item_list_find_index_a
/* Installed cluster menu_item_list_find_index_a (tools/cluster_install.py from output/cluster_pilot_zy/menu_item_list_find_index_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define MENU_ITEM_LIST_END (-0xf90000) /* terminator id 0xFF0F0000 */
/* menu_item_list_find_index_a */

/* "(list) index of the item with the current id, -1 at the 0xFF0F0000 terminator or a null list" */

/* Scans list for the nth_match-th entry equal to target_id and returns the element following it; -1 for a null list or the terminator (inferred). */
int menu_item_list_find_index_a_n(int *list, int target_id, int nth_match)

{
  int cur_id;
  int pos;
  int matches_seen;
  
  if (list == (int *)0x0) {
    return -1;
  }
  cur_id = *list;
  matches_seen = 0;
  pos = 0;
  do {
    if (cur_id == MENU_ITEM_LIST_END) {
      return -1;
    }
    if (cur_id == target_id) {
      if (nth_match == matches_seen) {
        return list[pos + 1];
      }
      matches_seen = matches_seen + 1;
    }
    cur_id = list[pos + 1];
    pos = pos + 1;
  } while( true );
}

/* entry thunk at the original address of menu_item_list_find_index_a: register inputs (edx,ebx,edi) -> natural C menu_item_list_find_index_a_n; ret 0 */
__declspec(naked) void menu_item_list_find_index_a(void)
{
    __asm {
        push edx
        push edi
        push ebx
        push edx
        call menu_item_list_find_index_a_n
        add esp, 12
        pop edx
        ret
    }
}
