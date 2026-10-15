/* functions of batch19.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch19.c is kept as it was */
#pragma auto_inline(off)
#include "rwrs.h"
extern int g_particles_65a9cc, g_particles_65a9d0, g_sky_atomic;
extern int g_course_objects_8c53ec, g_course_8c53e0, g_load_8c53e4, g_load_8c53e8;
extern char g_weapon_6a7ce8[], g_weapon_6a7c38[];
extern int g_particles_6c40b0, g_particles_6c40ac;
extern int g_course_warzone_6a2998, g_course_warzone_6a299c, g_course_warzone_6a29a0;
extern void particles_flush(int);
extern void particles_render(int);
extern void particles_render_c(void *);
extern void ptank_atomic_render(void);
extern void course_sky_atomic_render(void);
extern void fx_system_render(int);
extern void object_render(int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00421ca0
void vehicle_particles_render(void)
{
    particles_flush(g_particles_65a9cc);
    particles_flush(g_particles_65a9d0);
    RS_SET(RS_ZWRITE, 0);
    particles_render(g_particles_65a9d0);
    particles_render(g_particles_65a9cc);
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x00463270
void weapon_particles_render(void)
{
    RS_SET(RS_ZWRITE, 0);
    particles_render((int)g_weapon_6a7ce8);
    particles_render((int)g_weapon_6a7c38);
    RS_SET(RS_ZWRITE, 1);
}

// FUNCTION: MFL 0x00485900
void particles_render_all_no_zwrite(void)
{
    int i, off;
    RS_SET(RS_ZWRITE, 0);
    i = 0;
    if (0 < g_particles_6c40b0) {
        off = 0;
        do {
            particles_render(g_particles_6c40ac + off);
            i++;
            off += 0x40;
        } while (i < g_particles_6c40b0);
    }
    RS_SET(RS_ZWRITE, 1);
}
