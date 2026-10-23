/* MFL.exe render: functions moved verbatim from batch12.c, batch13.c, batch14.c, batch15.c, batch16.c, batch23.c, batch25.c, batch27.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_course_unload_88698c[];

// FUNCTION: MFL 0x00475760
int aux_camera_get_position(int vp)
{
    if (ui_get_menu_69d1ec() == 3) {
        if (vp == -1)
            return *(int *)(g_aux_camera + 4) + 0x40;
        return *(int *)(get_second_camera() + 4) + 0x40;
    }
    return *(int *)(g_aux_camera + 4) + 0x40;
}

// FUNCTION: MFL 0x00475860
int aux_camera_get_matrix(int vp)
{
    if (ui_get_menu_69d1ec() == 3) {
        if (vp == -1)
            return *(int *)(g_aux_camera + 4);
        return *(int *)(get_second_camera() + 4);
    }
    return *(int *)(g_aux_camera + 4);
}

// FUNCTION: MFL 0x00481790
__declspec(naked) void atomic_plugin_set_radius_squared(int atomic, float *r)   /* naked: original clobbers only EAX/ECX, see tools/clobber.py */
{
    __asm {
        mov eax, [esp+8]
        fld dword ptr [eax]
        mov eax, [esp+4]
        mov ecx, g_gfx_89de0c
        fld st(0)
        fmul st, st(1)
        fstp dword ptr [ecx+eax]
        fstp st(0)
        ret
    }
}

// FUNCTION: MFL 0x004817d0
void atomic_plugin_set_flags(char *atomic, int flags)
{
    *(int *)(g_gfx_89de0c + 0x20 + atomic) = flags;
}

// FUNCTION: MFL 0x00481a60
void object_set_pipeline_6c(char *obj)
{
    if (obj != 0) {
        int v = g_object_set_6c3eb0;
        if (v != 0)
            *(int *)(obj + 0x6c) = v;
    }
}

// FUNCTION: MFL 0x00481a80
void object_set_pipeline_7c(char *obj)
{
    if (obj != 0) {
        int v = g_object_set_6c3eb4;
        if (v != 0)
            *(int *)(obj + 0x7c) = v;
    }
}

// FUNCTION: MFL 0x00481aa0
void object_set_pipeline_08(char *obj)
{
    if (obj != 0) {
        int v = g_object_set_6c3eb8;
        if (v != 0)
            *(int *)(obj + 8) = v;
    }
}

// FUNCTION: MFL 0x00481b70
void material_plugin_construct(char *mat)
{
    int *p = (int *)(g_app_89de04 + mat);
    p[1] = 0;
    *p = 0;
}

// FUNCTION: MFL 0x00480e50
void gfx_frame_counters_rotate(void)
{
    g_gfx_frame_6c3ea4 = g_gfx_6c3ea0;
    g_gfx_6c3ea0 = 0;
}

// FUNCTION: MFL 0x00486860
void gfx_set_fx_matrix(struct Mat16 *p)
{
    if (p == 0)
        p = (struct Mat16 *)g_gfx_6c5148;
    *(struct Mat16 *)g_gfx_6c50d8 = *p;
}

// FUNCTION: MFL 0x004868d0
void gfx_set_params_6c5124(int *p)
{
    if (p == 0)
        p = g_gfx_62947c;
    g_particles_6c5124 = p[0];
    g_gfx_6c5128 = p[1];
    g_weapon_oil_6c512c = p[2];
    g_weapon_oil_6c5130 = p[3];
    g_weapon_oil_6c5134 = p[4];
    g_weapon_oil_6c5138 = p[5];
    g_weapon_oil_6c513c = p[6];
    g_weapon_oil_6c5140 = p[7];
}

// FUNCTION: MFL 0x00484680
void object_set_flag_4(char *obj, int on)
{
    if (obj != 0) {
        if (on != 0) {
            *(unsigned char *)(obj + 2) = *(unsigned char *)(obj + 2) | 4;
            return;
        }
        *(unsigned char *)(obj + 2) = *(unsigned char *)(obj + 2) & 0xfb;
    }
}

// FUNCTION: MFL 0x00484630
void material_flag_4_set(char *obj, int *on)
{
    int b = *(unsigned char *)(obj + 2);
    if (*on != 0) {
        *(unsigned char *)(obj + 2) = b | 4;
        return;
    }
    if ((b & 4) != 0)
        b = b - 4;
    *(unsigned char *)(obj + 2) = b;
}

// FUNCTION: MFL 0x00488d60
void table_clear_eb8(int *p)
{
    int n;
    for (n = 0xeb8; n != 0; n--)
        *p++ = 0;
}

// FUNCTION: MFL 0x00488d70
void table_clear_328f(int *p)
{
    int n;
    for (n = 0x328f; n != 0; n--)
        *p++ = 0;
}

// FUNCTION: MFL 0x00495c10
int viewport_get_field_702254(unsigned int i)
{
    if (i >= 4)
        return 0;
    return g_physics_702254[i];
}

// FUNCTION: MFL 0x00495c30
int viewport_get_camera(unsigned int i)
{
    if (i >= 4)
        return 0;
    return g_physics_702244[i];
}

// FUNCTION: MFL 0x00495e10
void world_sector_list_clear(void)
{
    g_physics_7036b0 = 0;
    g_physics_702a90 = 0;
    g_physics_702a94 = 0;
    g_physics_7036b4 = 0;
}

// FUNCTION: MFL 0x00497750
int pool_stack_pop_index(int *pool)
{
    int n = *pool;
    n--;
    *pool = n;
    if (n < 0) {
        *pool = 0;
        return 0;
    }
    return *(int *)(pool[2] + n * 4);
}

// FUNCTION: MFL 0x00496fc0
void *atomic_bbox_calculate_from_geometry(void *box, char *atomic)
{
    RwBBoxCalculate(box, *(void **)(*(char **)(*(char **)(atomic + 0x18) + 0x5c) + 0x14), *(int *)(*(char **)(atomic + 0x18) + 0x14));
    return box;
}

// FUNCTION: MFL 0x0049fee0
void materials_clear_textures_88602c(void)
{
    int *p = g_particles_88602c;
    do {
        RpMaterialSetTexture(*p, 0);
        p += 0x14;
    } while ((int)p < ((int)&g_course_unload_88698c));
}

// FUNCTION: MFL 0x0040d0b0
int atomic_set_envmap(int clump, void *tex)
{
    clump_set_envmap(clump, tex);
    return clump;
}

// FUNCTION: MFL 0x0044c5a0
void camera_hud_update(int on)
{
    if (on != 0) {
        if (ui_get_menu_69d1ec() == 3) {
            ui_get_8c8730();
            return;
        }
    }
}

// FUNCTION: MFL 0x00451c80
void camera_frame_ui(void)
{
    menu_scroll_clamp();
    camera_hud_update(g_car_8c5928);
    if (g_player_slots[3].extra != 0) {
        ai_debug_draw_lines();
        return;
    }
}
