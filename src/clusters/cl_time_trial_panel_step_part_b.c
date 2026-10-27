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

void time_trial_panel_step_part_b_n(int slots)

{
  int slot_ptr;
  int slot_index;
  RwReal dist;
  struct {
    float value;
    RwV3d delta;
    float local_matrix[16];  /* 4x4 matrix, position at [12..14] */
  } frame;
  
  vehicle_get_matrix_local(0,frame.local_matrix);
  slot_index = 0;
  do {
    slot_ptr = *(int *)(slots + slot_index * 4);
    frame.delta.x = frame.local_matrix[12] - *(float *)(slot_ptr + 0x30);
    frame.delta.y = frame.local_matrix[13] - *(float *)(slot_ptr + 0x34);
    frame.delta.z = frame.local_matrix[14] - *(float *)(slot_ptr + 0x38);
    dist = RwV3dLength(&frame.delta);
    frame.value = dist * k_f_0_1;
    if (!(k_f_0_5 <= frame.value)) {
      if (!(frame.value >= k_f_0)) {
        frame.value = 0.0;
      }
    }
    else {
      frame.value = 0.5;
    }
    btb_panel_slot_set_value(slot_index,*(int *)&frame.value);
    slot_index = slot_index + 1;
  } while (slot_index < 2);
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
