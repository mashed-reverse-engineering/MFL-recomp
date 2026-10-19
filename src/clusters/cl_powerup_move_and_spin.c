// cluster-entries: powerup_move_and_spin
/* Installed cluster powerup_move_and_spin (tools/cluster_install.py from output/cluster_pilot6/powerup_move_and_spin.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/* DECLS-END */
/* Cluster powerup_move_and_spin: per frame step of an available powerup: the velocity is added to the position and damped (0.95), the frame is moved,
   the powerup spins about its axis; when it has nearly stopped it is pulled gently towards its rest point (0.05 of the direction to it). */
typedef struct PowerupBody {
    struct { int unknown_00; RwFrame *frame; } *object;   /* 0x00 */
    float angle;           /* 0x04 */
    float spin;            /* 0x08: angle step per frame */
    RwV3d axis;            /* 0x0c */
    char unknown_18[0x20];
    RwV3d position;        /* 0x38 */
    RwV3d velocity;        /* 0x44 */
} PowerupBody;

extern float k_f_0_95;
extern float k_f_1_0en5;
extern float k_f_0_05;

static float powerup_direction_and_distance_r(PowerupBody * p0, RwV3d * p1);
extern void powerup_direction_and_distance(void);
/* was: EDI = powerup */
void powerup_move_and_spin_n(PowerupBody *powerup)
{
    struct {
        float angle;           /* S+0x00 */
        RwV3d direction;       /* S+0x04 */
    } frame;
    RwFrame *rwframe = powerup->object->frame;
    float distance;

    powerup->position.x = powerup->velocity.x + powerup->position.x;
    powerup->position.y = powerup->velocity.y + powerup->position.y;
    powerup->position.z = powerup->velocity.z + powerup->position.z;
    powerup->velocity.x = powerup->velocity.x * k_f_0_95;
    powerup->velocity.y = powerup->velocity.y * k_f_0_95;
    powerup->velocity.z = powerup->velocity.z * k_f_0_95;
    RwFrameTranslate(rwframe, &powerup->position, rwCOMBINEREPLACE);
    frame.angle = powerup->spin + powerup->angle;
    powerup->angle = frame.angle;
    RwFrameRotate(rwframe, &powerup->axis, frame.angle, rwCOMBINEPRECONCAT);
    if (powerup->velocity.z * powerup->velocity.z + powerup->velocity.x * powerup->velocity.x + powerup->velocity.y * powerup->velocity.y < k_f_1_0en5) {
        distance = powerup_direction_and_distance_r(powerup, &frame.direction) * k_f_0_05;
        powerup->velocity.x = frame.direction.x * distance;
        powerup->velocity.y = frame.direction.y * distance;
        powerup->velocity.z = distance * frame.direction.z;
    }
}

/* powerup_direction_and_distance_r: C -> original powerup_direction_and_distance (registers eax,esi) */
__declspec(naked) float powerup_direction_and_distance_r(PowerupBody * p0, RwV3d * p1)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov esi, dword ptr [esp + 12]
        call powerup_direction_and_distance
        pop esi
        ret
    }
}

/* entry thunk at the original address of powerup_move_and_spin: register inputs (edi) -> natural C powerup_move_and_spin_n; ret 0 */
__declspec(naked) void powerup_move_and_spin(void)
{
    __asm {
        push edi
        call powerup_move_and_spin_n
        add esp, 4
        ret
    }
}
