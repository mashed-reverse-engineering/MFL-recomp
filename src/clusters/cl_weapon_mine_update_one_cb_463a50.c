// cluster-entries: weapon_mine_update_one_cb_463a50
/* Installed cluster weapon_mine_update_one_cb_463a50 (tools/cluster_install.py from output/cluster_pilot_kd/weapon_mine_update_one_cb_463a50.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049cb60*/ extern int sphere_vs_box_test(float *hit, float *param_3);
/* weapon_mine_update_one_cb_463a50 */

/* function pointer pushed/stored by weapon_mine_update_one (MOV dword ptr [ESP + 0x40],0x463a50) */

void weapon_mine_update_one_cb_463a50(float *trigger, int hit)

{
  int obj; /* record the trigger belongs to (pointer stored in a float slot of the trigger) */
  int overlap;
  
  obj = *(int *)&trigger[5];
  if ((*(int *)(hit + 0x10) == 0x15) &&
     (overlap = *(int *)(hit + 0x14) + 0x68,
     overlap = sphere_vs_box_test((float *)overlap,trigger), overlap == 0)) {
    return;
  }
  *(undefined4 *)(obj + 0x60) = 4;
  return;
}

