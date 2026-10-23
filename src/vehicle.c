/* MFL.exe vehicle: functions moved verbatim from batch26.c, batch8.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00425ce0
void vehicle_prepare_clear_660568(void)
{
    if (g_game_660568 != 0)
        g_game_660568 = 0;
}

// FUNCTION: MFL 0x004265c0
void vehicle_render_state_set_6614e4(int v, int val)
{
    *(int *)((char *)&g_game_6614e4 + v * 0xf54) = val;
}

// FUNCTION: MFL 0x00426610
void vehicle_render_state_set_6614e8(int v, int val)
{
    *(int *)((char *)&g_game_6614e8 + v * 0xf54) = val;
}

// FUNCTION: MFL 0x004265e0
void vehicle_render_state_set_colour(int v, int *c)
{
    int *d = (int *)(g_game_6614a0 + v * 0xf54);
    d[0] = c[0];
    d[1] = c[1];
    d[2] = c[2];
    d[3] = c[3];
}

// FUNCTION: MFL 0x004220c0
void vehicle_get_position_148(int v, struct Vec4i *out)
{
    struct VehicleRecord *r = &g_vehicle_table[v];
    *out = *(struct Vec4i *)((char *)r + 0x248);
}

// FUNCTION: MFL 0x004220f0
void vehicle_get_position_158(int v, struct Vec4i *out)
{
    struct VehicleRecord *r = &g_vehicle_table[v];
    *out = *(struct Vec4i *)((char *)r + 0x258);
}

// FUNCTION: MFL 0x00422120
void vehicle_get_matrix_body(int v, struct Mat16 *out)
{
    *out = *(struct Mat16 *)RwFrameGetLTM(*(void **)(g_vehicle_table[v].part_frames[0x16] + 4));
}

// FUNCTION: MFL 0x004222a0
void vehicle_part_matrix(int v, int part, struct Mat16 *out)
{
    *out = *(struct Mat16 *)RwFrameGetLTM(*(void **)(g_game_659f00[v * 0xab + part] + 4));
}

// FUNCTION: MFL 0x004222e0
void vehicle_get_matrix_ltm_b(int v, struct Mat16 *out)
{
    struct VehicleRecord *r = &g_vehicle_table[v];
    *out = *(struct Mat16 *)RwFrameGetLTM(*(void **)(r->frame + 4));
}

// FUNCTION: MFL 0x00422320
void vehicle_get_matrix_local(int v, struct Mat16 *out)
{
    struct VehicleRecord *r = &g_vehicle_table[v];
    *out = *(struct Mat16 *)(*(char **)(r->frame + 4) + 0x10);
}

// FUNCTION: MFL 0x00422350
void vehicle_part_ltm(int v, int part)
{
    RwFrameGetLTM(*(void **)(g_game_659f00[v * 0xab + part] + 4));
}

// FUNCTION: MFL 0x00422440
int vehicle_part_id_has_damage_model(int id)
{
    if (g_load_64d890 == 0) {
        switch (id) {
        case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf: case 0x25: case 0x26:
        case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30: case 0x4c: case 100: case 0x65:
        case 0x66: case 0x67:
            return 1;
        }
    }
    return 0;
}
