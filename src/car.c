/* MFL.exe car: functions moved verbatim from batch12.c, batch22.c, batch8.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_car_damage_65c0c8[];
extern char g_car_damage_65c0e4[];
extern char player_car_0__matrices_0_[];

// FUNCTION: MFL 0x0047a640
int car_deactivate(unsigned int car)
{
    struct Car *c;
    int t = g_last_frame_ticks;
    if (car >= 0x10)
        return 0;
    c = &player_car[car];
    c->active = 0;
    c->field_b10 = t;
    return 1;
}

// FUNCTION: MFL 0x0047a830
unsigned int car_is_active(unsigned int car)
{
    if (car >= 0x10)
        return 0xffffffff;
    return player_car[car].active;
}

// FUNCTION: MFL 0x0047ac30
int car_get_mode(unsigned int car, unsigned int *mode, unsigned int *mode2)
{
    if (car >= 0x10)
        return 0;
    *mode = player_car[car].mode;
    *mode2 = player_car[car].mode2;
    return 1;
}

// FUNCTION: MFL 0x0047b750
int car_get_speed(float *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    *out = player_car[car].speed;
    return 1;
}

// FUNCTION: MFL 0x0047b7c0
int car_set_field_144(int *v, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    player_car[car].velocity_input[0] = ((float *)v)[0];
    player_car[car].velocity_input[1] = ((float *)v)[1];
    player_car[car].velocity_input[2] = ((float *)v)[2];
    return 1;
}

// FUNCTION: MFL 0x0047b520
int car_current_matrix(int *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    *out = player_car[car].matrix_current * 0x40 + ((int)&player_car_0__matrices_0_) + car * 0xd04;
    return 1;
}

// FUNCTION: MFL 0x00477130
int car_wheel_contact_find_index(int a, struct Car *car)
{
    int i = 0;
    int *p = car->contact_key;
    while (p[0x20] == 0 || *(int *)(a + 0x34) != *p) {   /* p[0x20] = contact_valid[i] */
        i++;
        p++;
        if (i >= 0x20)
            return 0;
    }
    return 1;
}

// FUNCTION: MFL 0x004241b0
void car_damage_smoke_reset(void)
{
    unsigned int *p = g_car_damage_65aae4;
    do {
        *p = 0xbf800000;
        p += 0xb;
    } while ((int)p < ((int)&g_car_damage_65c0e4));
    *(int *)&g_car_damage_65c0c8 = 0;
}
