/* MFL.exe car_np: functions moved verbatim from car_accessors.c, car_wheel_accessors.c, game_misc2.c, ui_misc.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0047b6e0
int car_get_velocity(float *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    out[0] = player_car[car].velocity[0];
    out[1] = player_car[car].velocity[1];
    out[2] = player_car[car].velocity[2];
    return 1;
}

// FUNCTION: MFL 0x0047ac60
int car_set_mode(unsigned int car, unsigned int mode, unsigned int mode2)
{
    if (car >= 0x10)
        return 0;
    player_car[car].mode = mode;
    player_car[car].mode2 = mode2;
    return 1;
}

// FUNCTION: MFL 0x0047a7f0
unsigned int car_get_controls_disabled(unsigned int car)
{
    if (car >= 0x10)
        return 0xffffffff;
    return player_car[car].controls_disabled;
}

// FUNCTION: MFL 0x0047a810
int car_set_controls_disabled(unsigned int car, unsigned int value)
{
    if (car >= 0x10)
        return -1;
    player_car[car].controls_disabled = value;
    return 0;
}

// FUNCTION: MFL 0x0047a750
int car_get_field_b10(int car, int *out)
{
    if (car >= 0x10)
        return 0;
    *out = player_car[car].field_b10;
    return 1;
}

// FUNCTION: MFL 0x0047a780
int car_add_field_b10(int car, int delta)
{
    if (car >= 0x10)
        return 0;
    player_car[car].field_b10 += delta;
    return 1;
}

// FUNCTION: MFL 0x0047a7b0
int car_get_field_bf8(int car)
{
    if (car >= 0x10)
        return 0;
    return player_car[car].turbo_state;
}

// FUNCTION: MFL 0x0047a7d0
int car_get_field_bf4(int car)
{
    if (car >= 0x10)
        return 0;
    return player_car[car].turbo_charge;
}

// FUNCTION: MFL 0x0047bc80
void car_set_field_8(int car, int vehicle_id)
{
    player_car[car].vehicle_id = vehicle_id;
}

// FUNCTION: MFL 0x0047bc60
int car_get_field_8(int car)
{
    return player_car[car].vehicle_id;
}

// FUNCTION: MFL 0x0047be10
void car_set_field_154(int car, float scale)
{
    player_car[car].speed_scale = scale;
}

// FUNCTION: MFL 0x0047b720
int car_get_field_b0c(int *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    *out = player_car[car].field_b0c;
    return 1;
}

// FUNCTION: MFL 0x0047a340
int car_get_field_af4(float *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    out[0] = player_car[car].field_af4[0];
    out[1] = player_car[car].field_af4[1];
    out[2] = player_car[car].field_af4[2];
    return 1;
}

// FUNCTION: MFL 0x0047a3c0
int car_wheel_get_field_3c(int *out, unsigned int car, unsigned int wheel)
{
    if (car >= 0x10)
        return 0;
    if (wheel >= 4)
        return 0;
    *out = player_car[car].wheels[wheel].spin_rate;
    return 1;
}

// FUNCTION: MFL 0x0047b360
int car_wheel_get_field_88_a(int car, int wheel, int *out)
{
    if (car >= 0x10)
        return 0;
    if (wheel >= 4)
        return 0;
    *out = player_car[car].wheels[wheel].surface_color;
    return 1;
}

// FUNCTION: MFL 0x0047b3a0
int car_wheel_get_field_88_b(int *out, unsigned int car, unsigned int wheel)
{
    if (car >= 0x10)
        return 0;
    if (wheel >= 4)
        return 0;
    *out = player_car[car].wheels[wheel].surface_color;
    return 1;
}

// FUNCTION: MFL 0x0047b3e0
int car_wheel_get_field_30(int *out, unsigned int car, unsigned int wheel)
{
    if (car >= 0x10)
        return 0;
    if (wheel >= 4)
        return 0;
    *out = player_car[car].wheels[wheel].in_contact;
    return 1;
}

// FUNCTION: MFL 0x0047b780
int car_get_vec3_9c8(int *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    out[0] = *(int *)&player_car[car].up[0];
    out[1] = *(int *)&player_car[car].up[1];
    out[2] = *(int *)&player_car[car].up[2];
    return 1;
}

// FUNCTION: MFL 0x0047a470
int car_set_wheel_contact(int *value, unsigned int car, int index, int flag)
{
    if (car >= 0x10)
        return 0;
    player_car[car].contact_key[index] = *value;
    player_car[car].contact_valid[index] = flag;
    return 1;
}

float car_damage_sum_front_back(int car)
{
    struct CarDamage *q = &car_damage_front_alias[car];
    return car_damage[car].back + q->front;
}

// FUNCTION: MFL 0x00423fa0
int car_damage_get_front(int car, int *out)
{
    struct CarDamage *d = &car_damage[car];
    if (out)
        *out = *(int *)&d->back;
    return d->back_state;
}

// FUNCTION: MFL 0x00423fc0
int car_damage_get_back(int car, int *out)
{
    struct CarDamage *d = &car_damage[car];
    if (out)
        *out = *(int *)&d->front;
    return d->front_state;
}

// FUNCTION: MFL 0x0047a380
int car_get_field_b00(int *out, unsigned int car)
{
    if (car >= 0x10)
        return 0;
    out[0] = *(int *)((char *)&player_car[car] + 0xb00);
    out[1] = *(int *)((char *)&player_car[car] + 0xb04);
    out[2] = *(int *)((char *)&player_car[car] + 0xb08);
    return 1;
}

// FUNCTION: MFL 0x00479ae0
int car_get_bounds_point(float *out, unsigned int car, int index)
{
    if (car >= 0x10)
        return 0;
    if (index >= 0x12)
        return 0;
    {
        float *f = (float *)&player_car[car];
        out[0] = f[index * 3 + 0x18];
        out[1] = f[index * 3 + 0x19];
        out[2] = f[index * 3 + 0x1a];
    }
    return 1;
}
