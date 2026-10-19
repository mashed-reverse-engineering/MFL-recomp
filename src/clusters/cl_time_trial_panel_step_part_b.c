// cluster-entries: time_trial_panel_step_part_b
/* Installed cluster time_trial_panel_step_part_b (tools/cluster_install.py from output/cluster_pilot11/time_trial_panel_step_part_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0041d630*/ extern undefined __cdecl btb_panel_slot_set_value(undefined4 p0, undefined4 p1);
/*00422320*/ extern int vehicle_get_matrix_local();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005ddc28*/ extern float k_f_0;
/* time_trial_panel_step_part_b */

/* WARNING: Unknown calling convention */
/* "per slot distance to the local vehicle matrix *0.1 clamped to [0,0.5]" */

void time_trial_panel_step_part_b_n(int in_edi)

{
  int iVar1;
  int iVar2;
  RwReal RVar3;
  struct {
    float local_50;
    RwV3d local_4c;
    float local_40[16];  /* 4x4 matrix, position at [12..14] */
  } frame;
  
  vehicle_get_matrix_local(0,frame.local_40);
  iVar2 = 0;
  do {
    iVar1 = *(int *)(in_edi + iVar2 * 4);
    frame.local_4c.x = frame.local_40[12] - *(float *)(iVar1 + 0x30);
    frame.local_4c.y = frame.local_40[13] - *(float *)(iVar1 + 0x34);
    frame.local_4c.z = frame.local_40[14] - *(float *)(iVar1 + 0x38);
    RVar3 = RwV3dLength(&frame.local_4c);
    frame.local_50 = RVar3 * k_f_0_1;
    if (k_f_0_5 > frame.local_50) {
      if (frame.local_50 < k_f_0) {
        frame.local_50 = 0.0;
      }
    }
    else {
      frame.local_50 = 0.5;
    }
    btb_panel_slot_set_value(iVar2,*(int *)&frame.local_50);
    iVar2 = iVar2 + 1;
  } while (iVar2 < 2);
  return;
}

/* entry thunk at the original address of time_trial_panel_step_part_b: register inputs (edi) -> natural C time_trial_panel_step_part_b_n; ret 0 */
__declspec(naked) void time_trial_panel_step_part_b(void)
{
    __asm {
        push edi
        call time_trial_panel_step_part_b_n
        add esp, 4
        ret
    }
}
