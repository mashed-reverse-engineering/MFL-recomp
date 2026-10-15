/* MFL.exe effects: functions moved verbatim from batch14.c, batch15.c, batch17.c, batch18.c, batch19.c, batch20.c, batch23.c, batch24.c, batch5.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_app_886980[];
extern char g_app_886984[];
extern char g_gfx_6c5e18[];

// FUNCTION: MFL 0x004867f0
void particles_set_position(struct V3i *p)
{
    int a, b, c;
    if (p == 0)
        p = &g_particles_set_6c5198;
    a = p->x;
    b = p->y;
    c = p->z;
    g_particles_6c5118.x = a;
    g_particles_6c5118.y = b;
    g_particles_6c5118.z = c;
}

// FUNCTION: MFL 0x00486820
void particles_set_size(struct V2i *p)
{
    if (p == 0)
        p = &g_gfx_629460;
    g_particles_6c50cc = *p;
}

// FUNCTION: MFL 0x00486840
void particles_set_color(int *p)
{
    int *d;
    if (p == 0) {
        d = &g_gfx_629468;
        g_particles_6c5144 = *d;
        return;
    }
    g_particles_6c5144 = *p;
}

// FUNCTION: MFL 0x00487e80
void effect_pool_set_field_14(int i, int v)
{
    *(int *)((char *)g_effect_pool_6c5e14 + i * 0x2c0) = v;
}

// FUNCTION: MFL 0x00487ea0
void effect_pool_set_field_1c(int i, int v)
{
    *(int *)(i * 0x2c0 + ((int)&g_gfx_6c5e18 + 0x4)) = v;
}

// FUNCTION: MFL 0x004978b0
void fx_pool_destroy(void)
{
    if (g_particles_7519b8 != 0) {
        pool_destroy(g_particles_7519b8);
        g_particles_7519b8 = 0;
    }
}

// FUNCTION: MFL 0x004982b0
void fx_atomics_render(void)
{
    (*(void (**)(int))(g_fx_atomic_a + 0x48))(g_fx_atomic_a);
    (*(void (**)(int))(g_fx_atomic_b + 0x48))(g_fx_atomic_b);
}

// FUNCTION: MFL 0x0049a980
int fx_slot_release(int slot)
{
    if (slot >= 0x80)
        return 0;
    g_particles_8869b4[slot * 0xe] = 0;
    g_particles_8869cc[slot * 0xe] = 0;
    g_particles_7596a4 = 0;
    return 1;
}

// FUNCTION: MFL 0x0049af00
void fx_slot_set_field_b8(int slot, int v)
{
    g_particles_8869b8[slot * 0xe] = v;
}

// FUNCTION: MFL 0x00498e20
void fx_particles_destroy_two(void)
{
    particles_destroy(g_load_752180);
    particles_destroy(g_sound_7521c0);
}

// FUNCTION: MFL 0x00498330
void fx_render(void)
{
    RS_SET(8, 0);
    fx_atomics_render();
    if (0 < g_particles_7519b4)
        fx_render_additive_overlay();
    RS_SET(8, 1);
}

// FUNCTION: MFL 0x0049b6b0
void fx_ptank_render_a(void)
{
    if (g_fx_ptank_atomic_a != 0) {
        RS_SET(8, 0);
        RS_SET(6, 1);
        (*(void (**)(int))(g_fx_ptank_atomic_a + 0x48))(g_fx_ptank_atomic_a);
        RS_SET(8, 1);
        RS_SET(6, 1);
    }
}

// FUNCTION: MFL 0x0048e300
void ptank_atomic_render(void)
{
    if (g_ptank_atomic != 0 && g_data_6f6934 != 0)
        (*(void (**)(int))(g_ptank_atomic + 0x48))(g_ptank_atomic);
}

// FUNCTION: MFL 0x0049c0f0
void fx_system_render_no_zwrite(int sys)
{
    RS_SET(RS_ZWRITE, 0);
    fx_system_render(*(int *)(sys + 0x28));
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x00498d70
void fx_systems_render(void)
{
    int *p;
    int *q;
    RS_SET(RS_ZWRITE, 0);
    p = g_particles_752184;
    do {
        q = p - 0x7a;
        p = p - 0x7a;
        fx_system_render(*q);
    } while (p != g_particles_7519e4);
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x0049f440
void fx_style_init_a(struct FxStyle *p)
{
    p->w0 = 0x3f99999a;
    p->c[0] = 0x92;
    p->c[1] = 0x74;
    p->c[2] = 0x48;
    p->c[3] = 0x80;
    p->w[7] = 0x40400000;
}

// FUNCTION: MFL 0x0049f470
void fx_style_init_b(struct FxStyle *p)
{
    p->w0 = 0x3f8ccccd;
    p->c[0] = 0x92;
    p->c[1] = 0x74;
    p->c[2] = 0x48;
    p->c[3] = 100;
    p->w[7] = 0x4019999a;
}

// FUNCTION: MFL 0x0049f4a0
void fx_style_init_c(struct FxStyle *p)
{
    p->w0 = 0x3f19999a;
    p->c[0] = 0xfa;
    p->c[1] = 0xcb;
    p->c[2] = 0x7e;
    p->c[3] = 0x5a;
    p->w[13] = 0x3f4ccccd;
    p->w[12] = 0xfffffff1;
    p->w[7] = 0x40000000;
}

// FUNCTION: MFL 0x0049f4d0
void fx_style_init_d(struct FxStyle *p)
{
    p->c[0] = 0xa0;
    p->c[1] = 0xa0;
    p->c[2] = 0xa0;
    p->w0 = 0x3f4ccccd;
    p->c[3] = 100;
    p->w[8] = 0x3f000000;
    p->w[9] = 0x3f000000;
    p->w[7] = 0x3fa66666;
}

// FUNCTION: MFL 0x0049f590
void fx_style_init_e(struct FxStyle *p)
{
    p->c[0] = 0xff;
    p->c[1] = 0xff;
    p->c[2] = 0xff;
    p->w0 = 0x3f99999a;
    p->c[3] = 0xa0;
    p->w[10] = 2;
    p->w[8] = 0x3ecccccd;
    p->w[9] = 0x3ecccccd;
    p->w[7] = 0x40400000;
}

// FUNCTION: MFL 0x0049f5d0
void fx_style_init_f(struct FxStyle *p)
{
    p->w0 = 0x3f800000;
    p->c[0] = 0x50;
    p->c[1] = 0x3c;
    p->c[2] = 0x28;
    p->c[3] = 0x96;
    p->w[7] = 0x40400000;
}

// FUNCTION: MFL 0x0049f600
void fx_style_init_g(struct FxStyle *p)
{
    p->c[0] = 0x96;
    p->c[1] = 0x96;
    p->c[2] = 0x96;
    p->w0 = 0x3f800000;
    p->c[3] = 0x69;
    p->w[12] = 0x19;
    p->w[7] = 0x40000000;
    p->w[8] = 0x3ee66666;
    p->w[9] = 0x3ee66666;
}

// FUNCTION: MFL 0x0049f640
void fx_style_init_h(struct FxStyle *p)
{
    p->c[0] = 0xeb;
    p->c[1] = 0xeb;
    p->c[2] = 0xeb;
    p->w[8] = 0x3ee66666;
    p->w[9] = 0x3ee66666;
    p->w0 = 0x3f800000;
    p->c[3] = 0x78;
    p->w[7] = 0x40000000;
    p->w[12] = 0xf;
    p->w[11] = p->w[11] | 0x800;
}

// FUNCTION: MFL 0x0049f680
void fx_style_init_i(struct FxStyle *p)
{
    p->w[8] = 0x3e19999a;
    p->w[9] = 0x3e19999a;
    p->w0 = 0x3f666666;
    p->c[0] = 0xbd;
    p->c[1] = 0x81;
    p->c[2] = 0x58;
    p->c[3] = 0xff;
    p->w[7] = 0x3f800000;
    p->w[12] = 0xffffffc4;
    p->w[4] = 0;
    p->w[5] = 0xc0000000;
    p->w[6] = 0;
}

// FUNCTION: MFL 0x0049feb0
void fx_clumps_destroy(void)
{
    int *p = g_app_886024;
    do {
        RpMaterialSetTexture(p[2], 0);
        RpClumpDestroy(*p);
        p += 0x14;
    } while ((int)p < ((int)&g_app_886984));
}

// FUNCTION: MFL 0x004a03b0
void fx_clump_slots_clear(void)
{
    int *p = g_app_886020;
    do {
        *p = 0;
        p += 0x14;
    } while ((int)p < ((int)&g_app_886980));
}

// FUNCTION: MFL 0x004a0ef0
void rain_slots_clear(void)
{
    int i = 0;
    do {
        *(int *)(g_app_885820 + i) = 0;
        i += 0x20;
    } while (i < 0x4000);
}

// FUNCTION: MFL 0x004a1490
void fx_ptank_buffers_destroy(void)
{
    (*(void (**)(int))(RwEngineInstance + 0x10c))(g_app_885820);
    RwFrameDestroy(*(int *)(g_app_76e5ac + 4));
    RpAtomicDestroy(g_app_76e5ac);
    g_app_76e5a0 = 0;
}

// FUNCTION: MFL 0x004a14d0
void fx_ptank_render(void)
{
    if (g_app_76e5ac != 0 && g_app_76e5a8 != 0)
        (*(void (**)(int))(g_app_76e5ac + 0x48))(g_app_76e5ac);
}

// FUNCTION: MFL 0x004a0aa0
int rain_buffers_alloc_once(void)
{
    if (g_app_76e3c0 == 0)
        return 0;
    fx_buffers_alloc();
    return 1;
}

// FUNCTION: MFL 0x00407db0
void fx_arm_weapon_slots(int group)
{
    int i = 0;
    float *p;
    float v;
    if (0 < g_fx_651948[group]) {
        p = (float *)((char *)g_fx_pickups + group * 0x78);
        do {
            if (player_has_weapon(0) == 1) {
                *p = p[1];
            } else {
                v = *p - g_tick_seconds;
                *p = v;
                if (v <= k_f_0) {
                    *p = p[1];
                    powerup_set_enabled((int)p[2], 1);
                }
            }
            i++;
            p += 3;
        } while (i < g_fx_651948[group]);
    }
}
