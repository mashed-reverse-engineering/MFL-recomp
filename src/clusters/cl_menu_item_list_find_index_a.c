// cluster-entries: menu_item_list_find_index_a
/* Installed cluster menu_item_list_find_index_a (tools/cluster_install.py from output/cluster_pilot_zy/menu_item_list_find_index_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* menu_item_list_find_index_a */

/* WARNING: Unknown calling convention */
/* "(list) index of the item with the current id, -1 at the 0xFF0F0000 terminator or a null list" */

int menu_item_list_find_index_a_n(int *in_edx, int in_ebx, int in_edi)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  if (in_edx == (int *)0x0) {
    return -1;
  }
  iVar1 = *in_edx;
  iVar3 = 0;
  iVar2 = 0;
  do {
    if (iVar1 == -0xf90000) {
      return -1;
    }
    if (iVar1 == in_ebx) {
      if (in_edi == iVar3) {
        return in_edx[iVar2 + 1];
      }
      iVar3 = iVar3 + 1;
    }
    iVar1 = in_edx[iVar2 + 1];
    iVar2 = iVar2 + 1;
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
