// cluster-entries: vehicle_exhaust_flame_particle_add
/* Installed cluster vehicle_exhaust_flame_particle_add (tools/cluster_install.py from output/cluster_pilot8/vehicle_exhaust_flame_particle_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_exhaust_flame_particle_add: exhaust flame of a vehicle (flag 0x40): the flame matrix lies between two car part frames
   (rotated 95 degrees about x, scaled 0.75 / 2 / 1, shifted by half the part distance), its alpha is the light direction dotted with
   the flame axis (at least 0, times 64); the particle is added with that matrix. */
typedef struct LightObject {
    int unknown_00;
    RwFrame *frame;        /* 0x04 */
} LightObject;

extern float k_f_0_5;
extern float k_f_0;
extern float k_f_64;
extern int g_particles_65a9cc;
extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, RwV3d *axis, float angle, int combine);
extern void particles_set_color(unsigned char *rgba);
extern void gfx_set_fx_matrix(RwMatrix *matrix);
extern void particles_add(int system);

/* was: ESI = vehicle record; the light object is a stack argument of the original (the caller's frame slot) */
void vehicle_exhaust_flame_particle_add_n(VehicleRecord *vehicle, LightObject *light)
{
    struct {                 /* the locals in the order of the original frame (the harness compares the memory behind &colour) */
        unsigned char colour[4];
        float delta[3];
        RwV3d scale;
        RwMatrix rear;
        RwV3d axis;
        RwMatrix front;
    } f;
    float half_up, dot;
    RwMatrix *ltm;

    f.colour[0] = 0xff;
    f.colour[1] = 0xff;
    f.colour[2] = 0xff;
    f.colour[3] = 0xff;
    f.axis.x = 1.0f;
    f.axis.y = 0.0f;
    f.axis.z = 0.0f;
    f.scale.x = 0.75f;
    f.scale.y = 2.0f;
    f.scale.z = 1.0f;
    if ((vehicle->flags & VEHICLE_FLAG_40) == 0)
        return;
    ltm = RwFrameGetLTM(*(RwFrame **)(vehicle->part_frames[0x19] + 4));
    RwMatrixTransform(&f.front, ltm, rwCOMBINEREPLACE);
    ltm = RwFrameGetLTM(*(RwFrame **)(vehicle->part_frames[0x1c] + 4));
    RwMatrixTransform(&f.rear, ltm, rwCOMBINEREPLACE);
    f.delta[0] = f.front.pos.x - f.rear.pos.x;
    f.delta[1] = f.front.pos.y - f.rear.pos.y;
    f.delta[2] = f.front.pos.z - f.rear.pos.z;
    RwMatrixRotate(&f.rear, &f.axis, 95.0f, rwCOMBINEPRECONCAT);
    half_up = f.scale.y * k_f_0_5;
    f.rear.pos.x = f.delta[0] * k_f_0_5 + f.rear.up.x * half_up + f.rear.pos.x;
    f.rear.pos.y = f.delta[1] * k_f_0_5 + half_up * f.rear.up.y + f.rear.pos.y;
    f.rear.pos.z = f.delta[2] * k_f_0_5 + half_up * f.rear.up.z + f.rear.pos.z;
    RwMatrixScale(&f.rear, &f.scale, rwCOMBINEPRECONCAT);
    RwMatrixUpdate(&f.rear);
    dot = RwFrame_modelling(light->frame).at.y * f.rear.at.y + RwFrame_modelling(light->frame).at.x * f.rear.at.x + RwFrame_modelling(light->frame).at.z * f.rear.at.z;
    if (!(k_f_0 < dot))
        dot = k_f_0;
    f.colour[3] = (unsigned char)(int)(dot * k_f_64);
    particles_set_color(f.colour);
    gfx_set_fx_matrix(&f.rear);
    particles_add(g_particles_65a9cc);
}

/* entry thunk at the original address of vehicle_exhaust_flame_particle_add: register inputs (esi,s0) -> natural C vehicle_exhaust_flame_particle_add_n; ret 0 */
__declspec(naked) void vehicle_exhaust_flame_particle_add(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        call vehicle_exhaust_flame_particle_add_n
        add esp, 8
        ret
    }
}
