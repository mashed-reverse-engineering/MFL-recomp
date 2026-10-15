/* functions of batch20.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch20.c is kept as it was */
#pragma auto_inline(off)
#include "rwrs.h"
extern void RpClumpRender(int);
extern int g_course_arctic_8c5840[];
extern void fx_ptank_render(void);
extern int get_viewport_camera(int);
extern int g_app_886024[];
extern int g_app_76e3c0, g_course_render_76e3cc, g_app_76e3bc;
extern void d3d9_draw_with_transform(int, int);
extern int g_weapons_effects_6a95d4[], g_weapon_6a9474[];
extern char g_particles_6a93e0[];
extern void particles_render(void *);
extern int g_particles_752184[], g_particles_7519e4[];
extern void fx_system_render(int);
extern void course_sky_atomic_render(void);
extern char g_course_arctic_8c58a4[];
extern char g_app_886984[];
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0045c0e0
void course_superg_cb2(int w)
{
    if (*(int *)(w + 0xc110) != 0)
        RpClumpRender(*(int *)(w + 0xc110));
    if (*(int *)(w + 0xc11c) != 0)
        RpClumpRender(*(int *)(w + 0xc11c));
    RS_SET(RS_ZWRITE, 0);
    course_sky_atomic_render();
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x004a0370
void clumps_render_all_886024(void)
{
    int *p;
    RS_SET(RS_CULLMODE, 1);
    p = g_app_886024;
    do {
        if (p[-1] != 0)
            RpClumpRender(*p);
        p += 0x14;
    } while ((int)p < ((int)&g_app_886984));
}

// FUNCTION: MFL 0x004a07d0
void rain_render(void)
{
    if (g_app_76e3c0 != 0) {
        if (g_course_render_76e3cc == 0) {
            RS_SET(RS_ZTEST, 1);
            RS_SET(RS_ZWRITE, 1);
            RS_SET(12, 1);
            RS_SET(9, 2);
            g_course_render_76e3cc = 1;
        }
        RS_SET(RS_TEXTURERASTER, 0);
        d3d9_draw_with_transform(g_app_76e3bc, 0x380);
    }
}

// FUNCTION: MFL 0x00452a20
void course_arctic_cb2(void)
{
    int *p;
    get_viewport_camera(0);
    p = g_course_arctic_8c5840;
    do {
        RpClumpRender(*p);
        p++;
    } while ((int)p < ((int)&g_course_arctic_8c58a4));
    RS_SET(RS_ZWRITE, 0);
    RS_SET(RS_FOGENABLE, 0);
    fx_ptank_render();
    RS_SET(RS_SRCBLEND, 5);
    RS_SET(RS_DESTBLEND, 2);
    RS_SET(RS_SRCBLEND, 5);
    RS_SET(RS_DESTBLEND, 6);
    RS_SET(RS_FOGENABLE, 1);
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x00465f80
void weapon_slots_render_all(void)
{
    int *p;
    int *flag;
    RS_SET(RS_CULLMODE, 1);
    p = g_weapons_effects_6a95d4;
    do {
        flag = p - 0x27;
        p = p - 0x16;
        if (*flag != 0)
            RpClumpRender(*p);
    } while (p != g_weapon_6a9474);
    particles_render(g_particles_6a93e0);
    RS_SET(RS_CULLMODE, 2);
}
