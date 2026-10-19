// cluster-entries: car_damage_smoke_choose car_damage_sum_front_back
/* Installed cluster car_damage_smoke_choose (tools/cluster_install.py from output/cluster_pilot8/car_damage_smoke_choose.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster car_damage_smoke_choose: chooses the smoke colour (RGBA bytes) from the total damage of a car: below 12.5 light grey, below 25 bluish grey, else dark. */
extern float k_f_12_5;
extern float k_f_25;
typedef struct CarDamageRec {
    float damage[2];       /* 0x00: front, back */
    char unknown_08[0x20];
} CarDamageRec;
extern CarDamageRec car_damage[4];

/* the callee leaves ECX alone (it only uses EAX), so it is part of the cluster: a stubbed callee would clobber the colour pointer in ECX */
float car_damage_sum_front_back(int car)
{
    return car_damage[car].damage[0] + car_damage[car].damage[1];
}

/* was: EAX = car, ECX = colour (4 bytes); car_damage_sum_front_back leaves ECX alone, so ECX is an input of this function too */
void car_damage_smoke_choose_n(int car, unsigned char *colour)
{
    float damage = car_damage_sum_front_back(car);

    if (damage < k_f_12_5) {
        colour[3] = 0x78;
        colour[0] = 0xdc;
        colour[1] = 0xdc;
        colour[2] = 0xdc;
        return;
    }
    if (damage < k_f_25) {
        colour[0] = 0x80;
        colour[1] = 0x80;
        colour[2] = 0xa0;
        colour[3] = 0xc8;
        return;
    }
    colour[3] = 0xf0;
    colour[0] = 0x28;
    colour[1] = 0x28;
    colour[2] = 0x28;
}

/* entry thunk at the original address of car_damage_smoke_choose: register inputs (eax,ecx) -> natural C car_damage_smoke_choose_n; ret 0 */
__declspec(naked) void car_damage_smoke_choose(void)
{
    __asm {
        push ecx
        push eax
        call car_damage_smoke_choose_n
        add esp, 8
        ret
    }
}
