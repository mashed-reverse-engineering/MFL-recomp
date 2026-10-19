// cluster-entries: menu_item_list_find_index_current
/* Installed cluster menu_item_list_find_index_current (tools/cluster_install.py from output/cluster_pilot11/menu_item_list_find_index_current.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/* menu_item_list_find_index_current */

/* WARNING: Unknown calling convention */
/* "index of the id in EBX in the item list of the current menu" */

int menu_item_list_find_index_current_n(int in_ebx, int in_edi)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  piVar1 = (int *)g_menu_records[g_menu_current].item_list;
  iVar2 = *piVar1;
  iVar4 = 0;
  iVar3 = 0;
  do {
    if (iVar2 == -0xf90000) {
      return -1;
    }
    if (iVar2 == in_ebx) {
      if (in_edi == iVar4) {
        return piVar1[iVar3 + 1];
      }
      iVar4 = iVar4 + 1;
    }
    iVar2 = piVar1[iVar3 + 1];
    iVar3 = iVar3 + 1;
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
