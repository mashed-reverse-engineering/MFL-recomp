// cluster-entries: vehicle_headlight_particle_add
/* Installed cluster vehicle_headlight_particle_add (tools/cluster_install.py from output/cluster_pilot8/vehicle_headlight_particle_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_headlight_particle_add: headlight glow particle of a vehicle: the car frame matrix, rotated 90 degrees about x, moved
   by the per-model headlight offset and scaled by the per-model size (tables of 3 floats, one entry per 6 model ids); its alpha is the
   up vector's y (at least 0) times the vehicle's light factor. */
typedef struct VehicleModelRef {
    char unknown_00[0x80];
    int model_id;          /* 0x80 */
} VehicleModelRef;
typedef struct VehicleLightView {      /* the VehicleRecord field at 0x288 has no name in the header yet */
    char unknown_00[0x268];
    VehicleModelRef *model;      /* 0x268 */
    char *frame;                 /* 0x26c: object whose frame is at +4 */
    char unknown_270[0x18];
    float light_factor;          /* 0x288 */
} VehicleLightView;

extern float k_f_0;
extern float k_f_255;
extern float g_vehicle_render_607628[][3];      /* headlight scale per model group */
extern float g_vehicle_render_6076e0[][3];      /* headlight offset per model group */
extern int g_particles_65a9d0;
extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, RwV3d *axis, float angle, int combine);
extern void particles_set_color(unsigned char *rgba);
extern void gfx_set_fx_matrix(RwMatrix *matrix);
extern void particles_add(int system);

/* was: EDI = vehicle record */
void vehicle_headlight_particle_add_n(VehicleLightView *vehicle)
{
    struct {                 /* the locals in the order of the original frame */
        unsigned char colour[4];
        RwV3d axis;
        RwV3d offset;
        RwV3d scale;
        RwMatrix matrix;
    } f;
    RwMatrix *car;
    float dot;
    int group;

    f.colour[0] = 0;
    f.colour[1] = 0;
    f.colour[2] = 0;
    f.colour[3] = 0xff;
    f.axis.x = 1.0f;
    f.axis.y = 0.0f;
    f.axis.z = 0.0f;
    group = vehicle->model->model_id / 6;
    f.scale.x = g_vehicle_render_607628[group][0];
    f.scale.y = g_vehicle_render_607628[group][1];
    f.scale.z = g_vehicle_render_607628[group][2];
    f.offset.x = g_vehicle_render_6076e0[group][0];
    f.offset.y = g_vehicle_render_6076e0[group][1];
    f.offset.z = g_vehicle_render_6076e0[group][2];
    car = (RwMatrix *)(*(char **)(vehicle->frame + 4) + 0x10);
    RwMatrixTransform(&f.matrix, car, rwCOMBINEREPLACE);
    RwMatrixRotate(&f.matrix, &f.axis, 90.0f, rwCOMBINEPRECONCAT);
    RwMatrixTranslate(&f.matrix, &f.offset, rwCOMBINEPRECONCAT);
    RwMatrixScale(&f.matrix, &f.scale, rwCOMBINEPRECONCAT);
    RwMatrixUpdate(&f.matrix);
    dot = car->up.x * k_f_0 + car->up.y + car->up.z * k_f_0;
    if (k_f_0 >= dot)
        dot = k_f_0;
    dot = dot * vehicle->light_factor;      /* one product per statement, source order */
    f.colour[3] = (unsigned char)(int)(dot * k_f_255);
    particles_set_color(f.colour);
    gfx_set_fx_matrix(&f.matrix);
    particles_add(g_particles_65a9d0);
}

/* entry thunk at the original address of vehicle_headlight_particle_add: register inputs (edi) -> natural C vehicle_headlight_particle_add_n; ret 0 */
__declspec(naked) void vehicle_headlight_particle_add(void)
{
    __asm {
        push edi
        call vehicle_headlight_particle_add_n
        add esp, 4
        ret
    }
}
