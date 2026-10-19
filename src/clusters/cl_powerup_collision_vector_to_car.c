// cluster-entries: powerup_collision_vector_to_car
/* Installed cluster powerup_collision_vector_to_car (tools/cluster_install.py from output/cluster_pilot6/powerup_collision_vector_to_car.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* DECLS-END */
/* Cluster powerup_collision_vector_to_car: vector from the powerup's frame position to the position of the car matrix, normalised in place;
   returns the length (0 when the vector is tiny). */
typedef struct PowerupObject {
    struct { int unknown_00; char *frame; } *object;   /* 0x00: object with the RwFrame pointer at +4 (matrix position at frame+0x40) */
} PowerupObject;

extern float k_f_1_0en6;
extern void car_current_matrix(char **matrix, int car);

/* was: EAX = car, ESI = result vector, the powerup is the one stack argument; the length comes back in ST0 */
float powerup_collision_vector_to_car_n(int car, RwV3d *out, PowerupObject *powerup)
{
    struct {
        float length;          /* S+0x00 */
        char *car_matrix;      /* S+0x04 */
        float car_pos[3];      /* S+0x08 */
        float powerup_pos[3];  /* S+0x14 */
    } frame;
    float *pos;

    frame.length = 0.0f;
    car_current_matrix(&frame.car_matrix, car);
    pos = (float *)(frame.car_matrix + 0x30);
    frame.car_pos[0] = pos[0];
    frame.car_pos[1] = pos[1];
    frame.car_pos[2] = pos[2];
    pos = (float *)(powerup->object->frame + 0x40);
    frame.powerup_pos[0] = pos[0];
    frame.powerup_pos[1] = pos[1];
    frame.powerup_pos[2] = pos[2];
    out->x = frame.car_pos[0] - frame.powerup_pos[0];
    out->y = frame.car_pos[1] - frame.powerup_pos[1];
    out->z = frame.car_pos[2] - frame.powerup_pos[2];
    if (out->z * out->z + out->x * out->x + out->y * out->y > k_f_1_0en6) {
        frame.length = RwV3dLength(out);
        RwV3dNormalize(out, out);
    }
    return frame.length;
}

/* entry thunk at the original address of powerup_collision_vector_to_car: register inputs (eax,esi,s0) -> natural C powerup_collision_vector_to_car_n; ret 0 */
__declspec(naked) void powerup_collision_vector_to_car(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        push eax
        call powerup_collision_vector_to_car_n
        add esp, 12
        ret
    }
}
