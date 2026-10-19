// cluster-entries: damage_add_clamped car_set_damage car_damage_at_local_point
/* Installed cluster damage_add_clamped (tools/cluster_install.py from output/cluster_pilot5/damage_add_clamped.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster damage_add_clamped: per car damage values (10 floats per car at car_damage), clamped to 50.0. */
extern float car_damage[];
extern float float_50_0;

/* Adds amount to values[index] and clamps the sum to 50.0 (was: ECX = index, EDX = values, amount on the stack). */
void damage_add_clamped_n(int index, float *values, float amount)
{
    float sum = amount + values[index];
    values[index] = sum;
    if (sum > float_50_0) {
        values[index] = 50.0f;
    }
}

/* (car, value, side): adds value to damage slot `side` of the car. */
void car_set_damage(int car, float value, int side)
{
    damage_add_clamped_n(side, &car_damage[car * 10], value);
}

/* (car, amount, local position, ...): front part (0) when the point's y is above 0, else part 1. */
void car_damage_at_local_point(int car, float amount, float *local_pos)
{
    if (local_pos[2] > 0.0f) {
        damage_add_clamped_n(0, &car_damage[car * 10], amount);
        return;
    }
    damage_add_clamped_n(1, &car_damage[car * 10], amount);
}

/* entry thunk at the original address of damage_add_clamped: register inputs (ecx,edx,s0) -> natural C damage_add_clamped_n; ret 0 */
__declspec(naked) void damage_add_clamped(void)
{
    __asm {
        push ecx
        push edx
        push dword ptr [esp + 12]
        push edx
        push ecx
        call damage_add_clamped_n
        add esp, 12
        pop edx
        pop ecx
        ret
    }
}
