// cluster-entries: powerup_direction_and_distance
/* Installed cluster powerup_direction_and_distance (tools/cluster_install.py from output/cluster_pilot5/powerup_direction_and_distance.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* DECLS-END */
/* Cluster powerup_direction_and_distance: vector from the powerup's frame position to its stored rest point, normalised in place; returns the length (0 when the vector is tiny). */
typedef struct PowerupObject {
    struct { int unknown_00; char *frame; } *object;   /* 0x00: object with the RwFrame pointer at +4 (matrix position at frame+0x40) */
    char unknown_04[0x28];
    float rest[3];                                     /* 0x2c */
} PowerupObject;

extern float k_f_1_0en6;
extern float k_f_0;

/* was: EAX = powerup, ESI = result vector; the length comes back in ST0 */
float powerup_direction_and_distance_n(PowerupObject *powerup, RwV3d *out)
{
    float dist = 0.0f;
    float *pos = (float *)(powerup->object->frame + 0x40);
    float px = pos[0];
    float py = pos[1];
    float pz = pos[2];

    out->x = powerup->rest[0] - px;
    out->y = powerup->rest[1] - py;
    out->z = powerup->rest[2] - pz;
    if (out->x * out->x + out->y * out->y + out->z * out->z > k_f_1_0en6) {
        dist = RwV3dLength(out);
        RwV3dNormalize(out, out);
    }
    return dist;
}

/* entry thunk at the original address of powerup_direction_and_distance: register inputs (eax,esi) -> natural C powerup_direction_and_distance_n; ret 0 */
__declspec(naked) void powerup_direction_and_distance(void)
{
    __asm {
        push esi
        push eax
        call powerup_direction_and_distance_n
        add esp, 8
        ret
    }
}
