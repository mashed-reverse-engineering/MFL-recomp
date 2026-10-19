// cluster-entries: vehicle_marker_morph_update
/* Installed cluster vehicle_marker_morph_update (tools/cluster_install.py from output/cluster_pilot_hb1/vehicle_marker_morph_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00425e80*/ extern double noise_smooth_value();
/*0047a400*/ extern int car_get_wheel_average();
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*00555220*/ extern void * __cdecl RpMorphAtomicSetTime(void * p0, RwReal p1);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd7f0*/ extern float k_f_30;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb0c*/ extern float k_f_15;
/*005de1b4*/ extern float k_f_0_0625;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/*0065a9d4*/ extern float g_vehicle_setup_65a9d4;
/* vehicle_marker_morph_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "sets the morph target time from wheel average speed and rotates the frame" */

void vehicle_marker_morph_update_n(VehicleRecord *vehicle_record, float param_2)

{
  RwFrame *frame;
  float fVar1;
  float10 fVar2;
  struct {
    float local_8;
    float local_4;
  } frame_obj;
  
  if (vehicle_record->field_298 == 0) {
    car_get_wheel_average(&frame_obj.local_8,vehicle_record->index,0);
    frame_obj.local_8 = frame_obj.local_8 * k_f_0_05;
    fVar1 = float_1_0;
    if ((float_1_0 > frame_obj.local_8) && (fVar1 = frame_obj.local_8, frame_obj.local_8 < k_f_n1)) {
      fVar1 = k_f_n1;
    }
    frame_obj.local_8 = fVar1 * k_f_0_5;
    RpMorphAtomicSetTime(vehicle_record->part_frames[0x12],frame_obj.local_8 + k_f_0_5);
    frame = *(RwFrame **)(vehicle_record->part_frames[0x14] + 4);
    fVar2 = (float10)noise_smooth_value(*(int *)&g_vehicle_setup_65a9d4,0xff);
    frame_obj.local_4 = (float)(fVar2 - (float10)k_f_0_5);
    RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,frame_obj.local_4 * k_f_30,rwCOMBINEREPLACE);
    RwFrameRotate(frame,(RwV3d *)&g_car_62ac68,frame_obj.local_4 * k_f_15,rwCOMBINEPRECONCAT);
    g_vehicle_setup_65a9d4 = param_2 * k_f_0_0625 + g_vehicle_setup_65a9d4;
  }
  return;
}

/* entry thunk at the original address of vehicle_marker_morph_update: register inputs (eax,s0) -> natural C vehicle_marker_morph_update_n; ret 0 */
__declspec(naked) void vehicle_marker_morph_update(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        call vehicle_marker_morph_update_n
        add esp, 8
        ret
    }
}
