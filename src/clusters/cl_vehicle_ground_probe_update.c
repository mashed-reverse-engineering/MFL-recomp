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

/* probe: segment from 1/16 above to 15/16 below the car position, tested once every 4 ticks (per vehicle index); history holds the last 8
   ground-contact samples, size_factor = their average clamped to [0,1]. */
void vehicle_ground_probe_update_n(VehicleRecord *vehicle_record)

{
  float sum;
  float fVar2;
  int *shift_ptr;
  RwMatrix *ltm;
  int count;
  float *history = (float *)&vehicle_record->part_frames[0x80];      /* the last 8 ground samples (bits of floats) */
  struct {
    int hit_index;
    float hit_dist;
    RwReal start_x;
    float start_y;
    RwReal start_z;
    RwReal end_x;
    float end_y;
    RwReal end_z;
  } probe;
  
  probe.hit_index = 0;
  probe.hit_dist = 1.0;
  if (g_course_64d894 != 0) {
    vehicle_record->size_factor = 1.0;
    *(float *)((char *)vehicle_record + 0x288) = 1.0f;
    return;
  }
  count = 7;
  shift_ptr = vehicle_record->part_frames + 0x87;
  do {
    *shift_ptr = shift_ptr[-1];
    shift_ptr = shift_ptr + -1;
    count = count + -1;
  } while (count != 0);
  ltm = RwFrameGetLTM(*(RwFrame **)(vehicle_record->frame + 4));
  probe.start_x = (ltm->pos).x;
  probe.start_y = (ltm->pos).y + k_f_0_0625;
  probe.start_z = (ltm->pos).z;
  probe.end_x = (ltm->pos).x;
  probe.end_y = (ltm->pos).y - k_f_0_9375;
  probe.end_z = (ltm->pos).z;
  if ((g_tick_number & 3) == vehicle_record->index) {
    count = course_world_ray_hit_light_forward(&probe.start_x,&probe.hit_index,&probe.hit_dist);
    if (count != 0) {
      vehicle_record->part_frames[0x80] = probe.hit_index;
    }
    *(float *)((char *)vehicle_record + 0x288) = float_1_0 - probe.hit_dist;
  }
  vehicle_record->size_factor = 0.0;
  sum = history[0];
  vehicle_record->size_factor = sum;
  sum = sum + history[1];
  vehicle_record->size_factor = sum;
  sum = sum + history[2];
  vehicle_record->size_factor = sum;
  sum = sum + history[3];
  vehicle_record->size_factor = sum;
  sum = sum + history[4];
  vehicle_record->size_factor = sum;
  sum = sum + history[5];
  vehicle_record->size_factor = sum;
  sum = sum + history[6];
  vehicle_record->size_factor = sum;
  sum = (sum + history[7]) * k_f_0_125;
  vehicle_record->size_factor = sum;
  if (sum >= float_1_0)
    vehicle_record->size_factor = float_1_0;
  else if (!(sum >= k_f_0))      /* NaN stores 0 in the original (test ah,1; jne) */
    vehicle_record->size_factor = k_f_0;
  else
    vehicle_record->size_factor = sum;
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
