/* MFL.exe effects_np: functions moved verbatim from game_misc.c, misc3.c, panels.c, ui_misc.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_trail_653148[];

// FUNCTION: MFL 0x00406cf0
int fx_objects_all_exploded(void)
{
    if (g_fx_object_count == 0)
        return 0;
    return g_explosion_651aa4 == g_fx_object_count;
}

// FUNCTION: MFL 0x004146a0
int trail_get_value(int a, int b)
{
    return g_trail_653064[a * 0xb + b];
}

// FUNCTION: MFL 0x00408aa0
int fx_find_index_by_rwp_id(int id)
{
    int i = 0;
    int *p;
    if (0 < g_fx_object_count) {
        p = g_scene_651294;
        do {
            if (*p == id)
                return i;
            i++;
            p += 0x3b;
        } while (i < g_fx_object_count);
    }
    return -1;
}

void trail_get_triple(int a, int b, int *out)
{
    int *p = &g_trail_652fb0[(b + a * 2) * 3];
    out[0] = p[0];
    out[1] = p[1];
    out[2] = p[2];
}

// FUNCTION: MFL 0x004146c0
void trail_clumps_call_update(void)
{
    int *p = g_trail_653040;
    int obj;
    do {
        if ((*(unsigned char *)(p + 1) & 1) != 0 && (obj = *p) != 0)
            (**(void (**)(int))(obj + 0x48))(obj);
        p += 0xb;
    } while ((int)p < ((int)&g_trail_653148));
}

// FUNCTION: MFL 0x004162f0
void trail_set_param(int a, int b, int value)
{
    int i = b + a * 5;
    g_trail_653170[i * 3] = value;
    if (g_particles_653168[i * 3] == 0.0f)
        g_particles_653168[i * 3] = 0.01f;
}

// FUNCTION: MFL 0x004165a0
void trail_reset(void)
{
    float *p = g_particles_653168;
    int n;
    do {
        n = 5;
        do {
            p[2] = 0;
            p[0] = 0;
            p[1] = 0;
            p += 3;
            n--;
        } while (n);
    } while ((int)p < ((int)&g_trail_653258));
}
