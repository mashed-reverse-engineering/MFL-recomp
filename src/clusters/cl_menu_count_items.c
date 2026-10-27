// cluster-entries: menu_count_items
/* Installed cluster menu_count_items (tools/cluster_install.py from output/cluster_pilot_hb1/menu_count_items.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* menu_count_items */

/* "counts the entries of a menu item list up to its terminator" */

int menu_count_items_n(int *list)

{
  int *piVar1;
  int entry;
  int count;
  
  count = 0;
  if (list != (int *)0x0) {
    while( true ) {
      entry = *list;
      while (entry != -0xfa0000) {
        if (entry == -0xf90000) {
          return count;
        }
        piVar1 = list + 1;
        list = list + 1;
        entry = *piVar1;
      }
      if (*list == -0xf90000) break;
      count = count + 1;
      list = list + 1;
    }
  }
  return count;
}

/* entry thunk at the original address of menu_count_items: register inputs (edx) -> natural C menu_count_items_n; ret 0 */
__declspec(naked) void menu_count_items(void)
{
    __asm {
        push edx
        call menu_count_items_n
        add esp, 4
        ret
    }
}
