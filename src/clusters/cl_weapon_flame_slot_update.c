// cluster-entries: weapon_flame_slot_update
/* Installed cluster weapon_flame_slot_update (tools/cluster_install.py from output/cluster_pilot8/weapon_flame_slot_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster weapon_flame_slot_update: the flames of a flamethrower slot are kept in sets of 5 records; the active records (age < 1)
   are sorted so that the one farthest from the given point comes first (a selection sort with swaps through a temporary record). */
extern float float_1_0;

typedef struct FlameRecord {
    float position[3];     /* 0x00 */
    float unknown_0c[3];
    float age;             /* 0x18 */
    float unknown_1c[5];
    int active;            /* 0x30 */
} FlameRecord;
extern FlameRecord g_weapon_6a9728[][5];     /* the flame sets, indexed by slot * 5 + sub */

/* was: EAX = slot, ECX = sub index, stack = point */
void weapon_flame_slot_update_n(int slot, int sub, const float *point)
{
    struct {
        int outer;
        char pad_3c[8];
        FlameRecord temp;
    } frame;
    FlameRecord *set = g_weapon_6a9728[sub + slot * 5];
    FlameRecord *a, *b;
    float ax, ay, az, px, py, pz, a_distance;
    int j;

    for (frame.outer = 1; frame.outer < 6; frame.outer++) {
        a = &set[frame.outer - 1];
        if (a->active != 0 && !(a->age >= float_1_0)) {
            px = point[0];
            ax = a->position[0];
            ay = a->position[1];
            py = point[1];
            az = a->position[2];
            pz = point[2];
            a_distance = (pz - az) * (pz - az) + (px - ax) * (px - ax) + (py - ay) * (py - ay);      /* squared distance, kept as a float */
            for (j = frame.outer; j < 5; j++) {
                b = &set[j];
                if (b->active != 0 && !(b->age >= float_1_0) &&
                    a_distance <
                        (point[2] - b->position[2]) * (point[2] - b->position[2]) + (point[0] - b->position[0]) * (point[0] - b->position[0]) +
                            (point[1] - b->position[1]) * (point[1] - b->position[1])) {
                    frame.temp = *b;
                    *b = *a;
                    *a = frame.temp;
                }
            }
        }
    }
}

/* entry thunk at the original address of weapon_flame_slot_update: register inputs (eax,ecx,s0) -> natural C weapon_flame_slot_update_n; ret 0 */
__declspec(naked) void weapon_flame_slot_update(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        push eax
        call weapon_flame_slot_update_n
        add esp, 12
        ret
    }
}
