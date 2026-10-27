// cluster-entries: world_render_count_sectors_cb_495e30
/* Installed cluster world_render_count_sectors_cb_495e30 (tools/cluster_install.py from output/cluster_pilot_kd/world_render_count_sectors_cb_495e30.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00702a94*/ extern undefined4 g_physics_702a94;
/*007036b0*/ extern undefined4 g_physics_7036b0;
/* world_render_count_sectors_cb_495e30 */

/* function pointer pushed/stored by world_render_count_sectors (PUSH 0x495e30) */

static int world_sector_list_add_r(undefined4 p0);
extern void world_sector_list_add(void);
int world_render_count_sectors_cb_495e30(int param_1)

{
  g_physics_7036b0 = g_physics_7036b0 + (uint)*(ushort *)(param_1 + 0x84);
  g_physics_702a94 = g_physics_702a94 + 1;
  if (*(ushort *)(param_1 + 0x84) != 0) {
    world_sector_list_add_r(param_1);
  }
  return param_1;
}

/* world_sector_list_add_r: C -> original world_sector_list_add (registers ecx) */
__declspec(naked) int world_sector_list_add_r(undefined4 p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call world_sector_list_add
        ret
    }
}
