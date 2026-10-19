// cluster-entries: kc_panel_entry_render
/* Installed cluster kc_panel_entry_render (tools/cluster_install.py from output/cluster_pilot_hb1/kc_panel_entry_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* kc_panel_entry_render */

/*  */

void kc_panel_entry_render_n(int in_eax)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  
  if (*(int *)(in_eax + 100) != 0) {
    (**(code **)(*(int *)(in_eax + 0xe4) + 0x48))(*(int *)(in_eax + 0xe4));
  }
  if (*(int *)(in_eax + 8) != 0) {
    *(undefined4 *)(**(int **)(*(int *)(*(int *)(in_eax + 0x88) + 0x18) + 0x20) + 4) = 0xff323232;
    (**(code **)(*(int *)(in_eax + 0x88) + 0x48))(*(int *)(in_eax + 0x88));
  }
  if (*(int *)(in_eax + 0x14) != 0) {
    *(undefined4 *)(**(int **)(*(int *)(*(int *)(in_eax + 0x94) + 0x18) + 0x20) + 4) = 0xff323232;
    (**(code **)(*(int *)(in_eax + 0x94) + 0x48))(*(int *)(in_eax + 0x94));
  }
  iVar2 = -2;
  piVar4 = (int *)(in_eax + 0x80);
  iVar3 = 0x20;
  do {
    iVar1 = *piVar4;
    if (iVar1 != 0) {
      switch(iVar2) {
      case 0:
      case 3:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 0x17:
        break;
      default:
        if (piVar4[-0x20] != 0) {
          (**(code **)(iVar1 + 0x48))(iVar1);
        }
      }
    }
    iVar2 = iVar2 + 1;
    piVar4 = piVar4 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  if (*(int *)(in_eax + 0x1c) != 0) {
    (**(code **)(*(int *)(in_eax + 0x9c) + 0x48))(*(int *)(in_eax + 0x9c));
  }
  if (*(int *)(in_eax + 0x20) != 0) {
    (**(code **)(*(int *)(in_eax + 0xa0) + 0x48))(*(int *)(in_eax + 0xa0));
  }
  if (*(int *)(in_eax + 0x24) != 0) {
    (**(code **)(*(int *)(in_eax + 0xa4) + 0x48))(*(int *)(in_eax + 0xa4));
  }
  if (*(int *)(in_eax + 0x28) != 0) {
    (**(code **)(*(int *)(in_eax + 0xa8) + 0x48))(*(int *)(in_eax + 0xa8));
  }
  if (*(int *)(in_eax + 0x2c) != 0) {
    (**(code **)(*(int *)(in_eax + 0xac) + 0x48))(*(int *)(in_eax + 0xac));
  }
  if (*(int *)(in_eax + 0x30) != 0) {
    (**(code **)(*(int *)(in_eax + 0xb0) + 0x48))(*(int *)(in_eax + 0xb0));
  }
  return;
}

/* entry thunk at the original address of kc_panel_entry_render: register inputs (eax) -> natural C kc_panel_entry_render_n; ret 0 */
__declspec(naked) void kc_panel_entry_render(void)
{
    __asm {
        push eax
        call kc_panel_entry_render_n
        add esp, 4
        ret
    }
}
