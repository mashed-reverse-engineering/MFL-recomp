/* MFL.exe vehicle_np: functions moved verbatim from game_misc.c, game_misc2.c, vehicles_stats.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00422070
unsigned char vehicle_is_active(int v)
{
    return (unsigned char)g_game_65a194[v * 0x2ac] >> 3 & 1;
}

// FUNCTION: MFL 0x00422050
int vehicle_get_object_field_80(int v)
{
    return *(int *)(*(char **)(g_game_65a168 + v * 0x2ac) + 0x80);
}

// FUNCTION: MFL 0x00422280
int vehicle_part_frame(int v, int part)
{
    return g_game_659f00[v * 0xab + part];
}

// FUNCTION: MFL 0x00429c20
void vehicle_slot_set(int slot, int value)
{
    g_game_8c88a4[slot * 0x14] = value;
    g_game_8c88a0[slot * 0x14] = 0;
}

// FUNCTION: MFL 0x00421e20
void vehicle_set_active(int v, int on)
{
    struct VehicleRec *r = &vehicle_table[v];
    if (on != 0) {
        r->flags |= VEHICLE_FLAG_ACTIVE;
        return;
    }
    r->flags &= 0xfffffff7;
}

// FUNCTION: MFL 0x00421fe0
void vehicle_set_field_198(int v, int value)
{
    struct VehicleRec *r = &vehicle_table[v];
    r->field_198 = value;
    if (value == 0)
        r->field_198 = 1;
}

// FUNCTION: MFL 0x00422010
void vehicle_set_flag_400(int v, int on)
{
    struct VehicleRec *r = &vehicle_table[v];
    if (on != 0) {
        r->flags |= VEHICLE_FLAG_400;
        return;
    }
    r->flags &= 0xfffffbff;
}

// FUNCTION: MFL 0x00422150
void vehicle_get_flags_10(int v, unsigned int *a, unsigned int *b)
{
    struct VehicleRec *r = &vehicle_table[v];
    if (a)
        *a = r->flags & VEHICLE_FLAG_FRONT;
    if (b)
        *b = r->flags & VEHICLE_FLAG_BACK;
}

// FUNCTION: MFL 0x00422190
void vehicle_get_flag_400(int v, unsigned int *a)
{
    if (a)
        *a = vehicle_table[v].flags & VEHICLE_FLAG_400;
}

// FUNCTION: MFL 0x004221c0
float vehicle_get_float_184(int v)
{
    return vehicle_table[v].float_184;
}

// FUNCTION: MFL 0x00422380
int vehicle_get_frame_16c(int v)
{
    return vehicle_table[v].frame;
}

// FUNCTION: MFL 0x004223e0
int vehicle_get_field_18c(int v)
{
    return vehicle_table[v].field_18c;
}

// FUNCTION: MFL 0x00422400
int vehicle_get_active_flag(int v)
{
    return g_game_659d50[v];
}

// FUNCTION: MFL 0x00423210
void vehicle_set_scale(int v, float scale)
{
    struct VehicleRec *r = &vehicle_table[v];
    r->half_scale = scale * 0.5f;
    r->flags |= 2;
}
