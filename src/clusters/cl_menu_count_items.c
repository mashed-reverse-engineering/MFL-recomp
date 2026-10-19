// cluster-entries: menu_count_items
/* Installed cluster menu_count_items (tools/cluster_install.py from output/cluster_pilot_hb1/menu_count_items.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* menu_count_items */

/* WARNING: Unknown calling convention */
/* "counts the entries of a menu item list up to its terminator" */

int menu_count_items_n(int *in_edx)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (in_edx != (int *)0x0) {
    while( true ) {
      iVar2 = *in_edx;
      while (iVar2 != -0xfa0000) {
        if (iVar2 == -0xf90000) {
          return iVar3;
        }
        piVar1 = in_edx + 1;
        in_edx = in_edx + 1;
        iVar2 = *piVar1;
      }
      if (*in_edx == -0xf90000) break;
      iVar3 = iVar3 + 1;
      in_edx = in_edx + 1;
    }
  }
  return iVar3;
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
