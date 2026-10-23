/* MFL.exe misc_np: functions moved verbatim from game_globals.c, game_misc.c, game_misc2.c, gfx_misc.c, math_helpers.c, misc3.c, panels.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_particles_658490[];

// FUNCTION: MFL 0x0042ba10
int language_offset_store(void)
{
    g_game_7ef744 = g_language_offset;
    return 1;
}

// FUNCTION: MFL 0x004166b0
float noise_lookup(int a, int b)
{
    return g_noise_table_653298[(a + b * 4) & 0xfff];
}

// FUNCTION: MFL 0x00420990
void endpoint_panel_clear(void)
{
    g_hudpanel_659aa4 = 0;
    g_hudpanel_659aa8 = 0.0f;
}

int array_find_index(int key, int *array, int count)
{
    int i;
    for (i = 0; i < count; i++, array += 10)
        if (*array == key)
            return i;
    return -1;
}

// FUNCTION: MFL 0x00434bc0
int event_type_is_race_basic(void)
{
    if (g_event_type != EVENT_TIME_TRIAL_0 && g_event_type != EVENT_TIME_TRIAL_1 && g_event_type != EVENT_TIME_TRIAL_2)
        return 0;
    return 1;
}

// FUNCTION: MFL 0x0042b1e0
int split_sectors_reset(void)
{
    int i;
    for (i = 0; i < 6; i++)
        g_split_sectors[i] = -1;
    return 1;
}

void triangle_normal_cross(RwV3d *out, float *a, float *b, float *c)
{
    float ux = a[0] - b[0], uy = a[1] - b[1], uz = a[2] - b[2];
    float vx = c[0] - b[0], vy = c[1] - b[1], vz = c[2] - b[2];
    out->x = vy * uz - vz * uy;
    out->y = vz * ux - vx * uz;
    out->z = vx * uy - vy * ux;
    RwV3dNormalize(out, out);
    ((float *)out)[3] = -(out->y * a[1] + out->x * a[0] + out->z * a[2]);
}

int matrix_is_valid_small(char *m)
{
    return vec3_is_valid_small((float *)(m + 0x20)) && vec3_is_valid_small((float *)(m + 0x10)) &&
           vec3_is_valid_small((float *)m) && vec3_is_valid_small((float *)(m + 0x30));
}

// FUNCTION: MFL 0x00405ab0
void vec3_multiply_components(float *out, const float *a, const float *b)
{
    out[0] = a[0] * b[0];
    out[1] = a[1] * b[1];
    out[2] = a[2] * b[2];
}

// FUNCTION: MFL 0x00484760
float degrees_to_radians(float x)
{
    return x * 0.017453292f;
}

// FUNCTION: MFL 0x00482030
float dot_product_2d_clamped(const float *a, const float *b)
{
    float d = a[0] * b[0] + a[1] * b[1];
    if (d < -1.0f)
        return -1.0f;
    if (d > 1.0f)
        d = 1.0f;
    return d;
}

// FUNCTION: MFL 0x00413880
int current_course_id_with_mirror(void)
{
    int id = current_course_id();
    if (g_game_state_mirror.mirror_mode != 0)
        id = id + 1000;
    return id;
}

// FUNCTION: MFL 0x0041bd90
void hudpanel_render_all_slots(void)
{
    int *p = g_load_658300;
    do {
        if (p[1] != 0)
            (**(void (**)(int))(*p + 0x48))(*p);
        p += 0x19;
    } while ((int)p < ((int)&g_particles_658490));
}
