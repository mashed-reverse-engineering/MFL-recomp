// cluster-entries: vehicle_ground_probe_update
/* Installed cluster vehicle_ground_probe_update (tools/cluster_install.py from output/cluster_pilot8/vehicle_ground_probe_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042b2e0*/ extern int __cdecl course_world_ray_hit_light_forward(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b4*/ extern float k_f_0_0625;
/*005de3e8*/ extern float k_f_0_9375;
/*0064d894*/ extern undefined4 g_course_64d894;
/*007ef804*/ extern undefined4 g_tick_number;
/* vehicle_ground_probe_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "shifts the history of 7 samples and probes the ground below the car every 4th tick" */

void vehicle_ground_probe_update_n(VehicleRecord *vehicle_record)

{
  float fVar1;
  float fVar2;
  int *piVar3;
  RwMatrix *pRVar4;
  int iVar5;
  float *history = (float *)&vehicle_record->part_frames[0x80];      /* the last 8 ground samples (bits of floats) */
  struct {
    int local_20;
    float local_1c;
    RwReal local_18;
    float local_14;
    RwReal local_10;
    RwReal local_c;
    float local_8;
    RwReal local_4;
  } frame_obj;
  
  frame_obj.local_20 = 0;
  frame_obj.local_1c = 1.0;
  if (g_course_64d894 != 0) {
    vehicle_record->size_factor = 1.0;
    *(float *)((char *)vehicle_record + 0x288) = 1.0f;
    return;
  }
  iVar5 = 7;
  piVar3 = vehicle_record->part_frames + 0x87;
  do {
    *piVar3 = piVar3[-1];
    piVar3 = piVar3 + -1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  pRVar4 = RwFrameGetLTM(*(RwFrame **)(vehicle_record->frame + 4));
  frame_obj.local_18 = (pRVar4->pos).x;
  frame_obj.local_14 = (pRVar4->pos).y + k_f_0_0625;
  frame_obj.local_10 = (pRVar4->pos).z;
  frame_obj.local_c = (pRVar4->pos).x;
  frame_obj.local_8 = (pRVar4->pos).y - k_f_0_9375;
  frame_obj.local_4 = (pRVar4->pos).z;
  if ((g_tick_number & 3) == vehicle_record->index) {
    iVar5 = course_world_ray_hit_light_forward(&frame_obj.local_18,&frame_obj.local_20,&frame_obj.local_1c);
    if (iVar5 != 0) {
      vehicle_record->part_frames[0x80] = frame_obj.local_20;
    }
    *(float *)((char *)vehicle_record + 0x288) = float_1_0 - frame_obj.local_1c;
  }
  vehicle_record->size_factor = 0.0;
  fVar1 = history[0];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[1];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[2];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[3];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[4];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[5];
  vehicle_record->size_factor = fVar1;
  fVar1 = fVar1 + history[6];
  vehicle_record->size_factor = fVar1;
  fVar1 = (fVar1 + history[7]) * k_f_0_125;
  vehicle_record->size_factor = fVar1;
  if (fVar1 >= float_1_0)
    vehicle_record->size_factor = float_1_0;
  else if (fVar1 < k_f_0)
    vehicle_record->size_factor = k_f_0;
  else
    vehicle_record->size_factor = fVar1;
}

/* entry thunk at the original address of vehicle_ground_probe_update: register inputs (esi) -> natural C vehicle_ground_probe_update_n; ret 0 */
__declspec(naked) void vehicle_ground_probe_update(void)
{
    __asm {
        push esi
        call vehicle_ground_probe_update_n
        add esp, 4
        ret
    }
}
