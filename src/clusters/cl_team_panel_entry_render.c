// cluster-entries: team_panel_entry_render
/* Installed cluster team_panel_entry_render (tools/cluster_install.py from output/cluster_pilot10/team_panel_entry_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* team_panel_entry_render */

/* WARNING: Unknown calling convention */
/*  */

void team_panel_entry_render_n(TeamPanelEntry *panel)

{
  if (panel->visible[3] != 0) {
    (**(code **)(panel->part[3] + 0x48))((int)panel->part[3]);
  }
  if (panel->visible[2] != 0) {
    (**(code **)(panel->part[2] + 0x48))((int)panel->part[2]);
  }
  if (panel->visible[0] != 0) {
    (**(code **)(panel->part[0] + 0x48))((int)panel->part[0]);
  }
  if (panel->visible[1] != 0) {
    (**(code **)(panel->part[1] + 0x48))((int)panel->part[1]);
  }
  if (panel->visible[4] != 0) {
    (**(code **)(panel->part[4] + 0x48))((int)panel->part[4]);
  }
  if (panel->visible[28] != 0) {
    (**(code **)(panel->part[28] + 0x48))((int)panel->part[28]);
  }
  if (panel->visible[29] != 0) {
    (**(code **)(panel->part[29] + 0x48))((int)panel->part[29]);
  }
  if (panel->visible[30] != 0) {
    (**(code **)(panel->part[30] + 0x48))((int)panel->part[30]);
  }
  if (panel->visible[31] != 0) {
    (**(code **)(panel->part[31] + 0x48))((int)panel->part[31]);
  }
  if (panel->visible[32] != 0) {
    (**(code **)(panel->part[32] + 0x48))((int)panel->part[32]);
  }
  if (panel->visible[33] != 0) {
    (**(code **)(panel->part[33] + 0x48))((int)panel->part[33]);
  }
  if (panel->visible[22] != 0) {
    (**(code **)(panel->part[22] + 0x48))((int)panel->part[22]);
  }
  if (panel->visible[23] != 0) {
    (**(code **)(panel->part[23] + 0x48))((int)panel->part[23]);
  }
  if (panel->visible[24] != 0) {
    (**(code **)(panel->part[24] + 0x48))((int)panel->part[24]);
  }
  if (panel->visible[25] != 0) {
    (**(code **)(panel->part[25] + 0x48))((int)panel->part[25]);
  }
  if (panel->visible[26] != 0) {
    (**(code **)(panel->part[26] + 0x48))((int)panel->part[26]);
  }
  if (panel->visible[27] != 0) {
    (**(code **)(panel->part[27] + 0x48))((int)panel->part[27]);
  }
  if (panel->visible[11] != 0) {
    (**(code **)(panel->part[11] + 0x48))((int)panel->part[11]);
  }
  if (panel->visible[12] != 0) {
    (**(code **)(panel->part[12] + 0x48))((int)panel->part[12]);
  }
  if (panel->visible[13] != 0) {
    (**(code **)(panel->part[13] + 0x48))((int)panel->part[13]);
  }
  if (panel->visible[14] != 0) {
    (**(code **)(panel->part[14] + 0x48))((int)panel->part[14]);
  }
  if (panel->visible[15] != 0) {
    (**(code **)(panel->part[15] + 0x48))((int)panel->part[15]);
  }
  if (panel->visible[16] != 0) {
    (**(code **)(panel->part[16] + 0x48))((int)panel->part[16]);
  }
  if (panel->visible[5] != 0) {
    (**(code **)(panel->part[5] + 0x48))((int)panel->part[5]);
  }
  if (panel->visible[6] != 0) {
    (**(code **)(panel->part[6] + 0x48))((int)panel->part[6]);
  }
  if (panel->visible[7] != 0) {
    (**(code **)(panel->part[7] + 0x48))((int)panel->part[7]);
  }
  if (panel->visible[8] != 0) {
    (**(code **)(panel->part[8] + 0x48))((int)panel->part[8]);
  }
  if (panel->visible[9] != 0) {
    (**(code **)(panel->part[9] + 0x48))((int)panel->part[9]);
  }
  if (panel->visible[10] != 0) {
    (**(code **)(panel->part[10] + 0x48))((int)panel->part[10]);
  }
  return;
}

/* entry thunk at the original address of team_panel_entry_render: register inputs (eax) -> natural C team_panel_entry_render_n; ret 0 */
__declspec(naked) void team_panel_entry_render(void)
{
    __asm {
        push eax
        call team_panel_entry_render_n
        add esp, 4
        ret
    }
}
