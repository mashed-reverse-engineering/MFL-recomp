// cluster-entries: weapon_drum_update_one_cb_45fad0
/* Installed cluster weapon_drum_update_one_cb_45fad0 (tools/cluster_install.py from output/cluster_pilot_kd/weapon_drum_update_one_cb_45fad0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049cb60*/ extern int sphere_vs_box_test(float *param_2, float *param_3);
/* weapon_drum_update_one_cb_45fad0 */

/* function pointer pushed/stored by weapon_drum_update_one (MOV dword ptr [ESP + 0xf0],0x45fad0) */

void weapon_drum_update_one_cb_45fad0(float *param_1, int param_2)

{
  int obj; /* the drum the trigger belongs to (pointer stored in a float slot of the trigger) */
  int iVar2;
  
  obj = *(int *)&param_1[5];
  if (*(int *)(param_2 + 0x10) == 0x15) {
    iVar2 = *(int *)(param_2 + 0x14) + 0x68;
    iVar2 = sphere_vs_box_test((float *)iVar2,param_1);
    if (iVar2 != 0) {
      *(undefined4 *)(obj + 0x2c) = 3;
    }
  }
  return;
}

