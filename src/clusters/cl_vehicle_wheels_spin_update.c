// cluster-entries: vehicle_wheels_spin_update
/* Installed cluster vehicle_wheels_spin_update (tools/cluster_install.py from output/cluster_pilot8/vehicle_wheels_spin_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_wheels_spin_update: rotates the wheel frames of a vehicle (three or four wheels, depending on the vehicle type): the
   spin angle (kept in degrees, -360..360, in the vehicle record) advances by ticks / 3000 times the wheel rotation speed, the wheel
   frame's parent is turned by the steering angle about the up axis and moved back to its place. */
extern float k_f_360;
extern float k_f_n360;
extern float k_f_3_3333333en4;
extern float g_car_62ac68;       /* wheel axis */
extern float g_car_62ac74;       /* up axis */
extern void __cdecl car_wheel_get_field_3c(int *out, int car, int wheel);
extern void __cdecl car_get_wheel_average(float *out, int car, int wheel);
extern RwFrame *RwFrameRotate(RwFrame *frame, RwV3d *axis, float angle, int combine);

/* was: EDI = vehicle record, stack = ticks */
void vehicle_wheels_spin_update_n(VehicleRecord *vehicle, unsigned int ticks)
{
    float *spin = (float *)&vehicle->part_frames[0x88];      /* spin angle of the four wheels */
    struct {
        int wheel_rotation;
        float steering;
        unsigned int ticks_copy;
        RwV3d position;
    } frame;
    int wheel;

    for (wheel = 0; wheel < 4; wheel++) {
        int part;
        RwFrame *wheel_frame;
        RwFrame *parent;
        float angle;

        if (vehicle->field_298 == 0) {
            part = vehicle->part_frames[wheel];
        } else {
            if (vehicle->field_298 != 1)
                return;
            if (wheel == 3)
                continue;
            part = (wheel == 2) ? vehicle->part_frames[0x4b] : vehicle->part_frames[wheel + 0x21];
        }
        wheel_frame = *(RwFrame **)(part + 4);
        car_wheel_get_field_3c(&frame.wheel_rotation, vehicle->index, wheel);
        frame.ticks_copy = ticks;
        angle = (float)ticks * k_f_3_3333333en4;      /* one product per statement, source order */
        angle = angle * (float)frame.wheel_rotation;
        angle = angle + spin[wheel];
        spin[wheel] = angle;
        if (k_f_360 < angle)
            spin[wheel] = angle - k_f_360;
        if (spin[wheel] < k_f_n360)
            spin[wheel] = spin[wheel] + k_f_360;
        RwFrameRotate(wheel_frame, (RwV3d *)&g_car_62ac68, spin[wheel], rwCOMBINEREPLACE);
        car_get_wheel_average(&frame.steering, vehicle->index, wheel);
        parent = RwFrame_object_parent(wheel_frame);
        frame.position.x = (RwFrame_modelling(parent)).pos.x;
        frame.position.y = (RwFrame_modelling(parent)).pos.y;
        frame.position.z = (RwFrame_modelling(parent)).pos.z;
        RwFrameRotate(parent, (RwV3d *)&g_car_62ac74, frame.steering, rwCOMBINEREPLACE);
        RwFrameTranslate(parent, &frame.position, rwCOMBINEPOSTCONCAT);
    }
}

/* entry thunk at the original address of vehicle_wheels_spin_update: register inputs (edi,s0) -> natural C vehicle_wheels_spin_update_n; ret 0 */
__declspec(naked) void vehicle_wheels_spin_update(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edi
        call vehicle_wheels_spin_update_n
        add esp, 8
        ret
    }
}
