// cluster-entries: kc_panel_entry_render
/* Installed cluster kc_panel_entry_render (tools/cluster_install.py from output/cluster_pilot_hb1/kc_panel_entry_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* kc_panel_entry_render */

/*  */

/* same record as in cl_kc_panel_entry_update.c: visible[k] and part[k] describe panel part k */
typedef struct KcPanelEntry {
    int visible[32]; /* 0x0 */
    int part[32]; /* 0x80: part objects, vtable slot 0x48 draws one */
} KcPanelEntry;

void kc_panel_entry_render_n(KcPanelEntry *entry)

{
  int part_obj;
  int part_index_minus_2;
  int parts_left;
  int *part_ptr;
  
  if (entry->visible[25] != 0) {
    (**(code **)(entry->part[25] + 0x48))(entry->part[25]);
  }
  if (entry->visible[2] != 0) {
    *(undefined4 *)(**(int **)(*(int *)(entry->part[2] + 0x18) + 0x20) + 4) = 0xff323232;
    (**(code **)(entry->part[2] + 0x48))(entry->part[2]);
  }
  if (entry->visible[5] != 0) {
    *(undefined4 *)(**(int **)(*(int *)(entry->part[5] + 0x18) + 0x20) + 4) = 0xff323232;
    (**(code **)(entry->part[5] + 0x48))(entry->part[5]);
  }
  /* loop over all 32 parts: part_index_minus_2 = k-2 for part k; parts 2,5,7..12,25 are drawn explicitly outside the loop (the switch skips them); the others are drawn when visible[k] is set. */
  part_index_minus_2 = -2;
  part_ptr = (int *)entry->part;
  parts_left = 0x20;
  do {
    part_obj = *part_ptr;
    if (part_obj != 0) {
      switch(part_index_minus_2) {
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
        if (part_ptr[-0x20] != 0) {
          (**(code **)(part_obj + 0x48))(part_obj);
        }
      }
    }
    part_index_minus_2 = part_index_minus_2 + 1;
    part_ptr = part_ptr + 1;
    parts_left = parts_left + -1;
  } while (parts_left != 0);
  if (entry->visible[7] != 0) {
    (**(code **)(entry->part[7] + 0x48))(entry->part[7]);
  }
  if (entry->visible[8] != 0) {
    (**(code **)(entry->part[8] + 0x48))(entry->part[8]);
  }
  if (entry->visible[9] != 0) {
    (**(code **)(entry->part[9] + 0x48))(entry->part[9]);
  }
  if (entry->visible[10] != 0) {
    (**(code **)(entry->part[10] + 0x48))(entry->part[10]);
  }
  if (entry->visible[11] != 0) {
    (**(code **)(entry->part[11] + 0x48))(entry->part[11]);
  }
  if (entry->visible[12] != 0) {
    (**(code **)(entry->part[12] + 0x48))(entry->part[12]);
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
