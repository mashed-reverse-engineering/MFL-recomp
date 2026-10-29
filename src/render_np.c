/* MFL.exe render_np: functions moved verbatim from game_globals.c, gfx_misc.c, misc3.c, ui_misc.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_car_8c7440[];

// FUNCTION: MFL 0x00406270
void scene_object_select_clear(void)
{
    g_scene_select_timer = 0.0f;
    g_scene_select_param = 0;
    g_scene_selected_object = 0;
}

// FUNCTION: MFL 0x00486890
void gfx_set_params_6c5188(int *p)
{
    if (p == 0)
        p = g_gfx_62946c;
    g_gfx_6c5188 = p[0];
    g_gfx_6c518c = p[1];
    g_gfx_6c5190 = p[2];
    g_gfx_6c5194 = p[3];
}

// FUNCTION: MFL 0x0048a660
void gfx_set_vec3_6f2aa8(int x, int y, int z)
{
    g_gfx_6f2aa8 = x;
    g_gfx_6f2aac = y;
    g_gfx_6f2ab0 = z;
}

void scene_object_select_set(int object, int param)
{
    if (object != 0) {
        g_scene_select_timer = 0.0f;
        g_scene_selected_object = object;
        g_scene_select_param = param;
        return;
    }
    g_scene_selected_object = 0;
    g_scene_select_param = 0;
}

/* scene_object_select_is_active: now recomp/tier1/scene_object_select_is_active.c (byte identical) */

// FUNCTION: MFL 0x0044ba50
void camera_records_clear(void)
{
    int *p = g_camera_records_8c5940;
    do {
        *p = 0;
        p += 0x36;
    } while ((int)p < ((int)&g_car_8c7440));
    g_camera_records_8c7e10 = 0;
}
