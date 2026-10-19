// cluster-entries: vehicle_marker_steering_wheel_update
/* Installed cluster vehicle_marker_steering_wheel_update (tools/cluster_install.py from output/cluster_pilot8/vehicle_marker_steering_wheel_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0047a340*/ extern int car_get_field_af4();
/*0047a380*/ extern int car_get_field_b00();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005de198*/ extern float k_f_n100;
/*005de3e4*/ extern float k_f_40;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/* vehicle_marker_steering_wheel_update */

/* WARNING: Unknown calling convention */
/* "rotates the wheel frame from the car's af4/b00 fields" */

void vehicle_marker_steering_wheel_update_n(VehicleRecord *vehicle_record)

{
  RwFrame *frame;
  float angle;
  struct {
    RwV3d local_24;
    float local_18 [2];
    float local_10;
    float local_c [2];
    float local_4;
  } frame_obj;
  
  frame = *(RwFrame **)(vehicle_record->part_frames[0x13] + 4);
  car_get_field_af4(frame_obj.local_c,vehicle_record->index);
  car_get_field_b00(frame_obj.local_18,vehicle_record->index);
  frame_obj.local_24.x = (RwFrame_modelling(frame)).pos.x;
  angle = (frame_obj.local_10 + frame_obj.local_4) * k_f_n100;
  frame_obj.local_24.y = (RwFrame_modelling(frame)).pos.y;
  frame_obj.local_24.z = (RwFrame_modelling(frame)).pos.z;
  RwFrameRotate(frame,(RwV3d *)&g_weapon_62ac80,(frame_obj.local_18[0] + frame_obj.local_c[0]) * k_f_40,rwCOMBINEREPLACE
               );
  RwFrameRotate(frame,(RwV3d *)&g_car_62ac68,angle,rwCOMBINEPOSTCONCAT);
  RwFrameTranslate(frame,&frame_obj.local_24,rwCOMBINEPOSTCONCAT);
  return;
}

/* entry thunk at the original address of vehicle_marker_steering_wheel_update: register inputs (edi) -> natural C vehicle_marker_steering_wheel_update_n; ret 0 */
__declspec(naked) void vehicle_marker_steering_wheel_update(void)
{
    __asm {
        push edi
        call vehicle_marker_steering_wheel_update_n
        add esp, 4
        ret
    }
}
