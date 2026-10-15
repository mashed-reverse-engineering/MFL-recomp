/* functions of batch18.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch18.c is kept as it was */
#pragma auto_inline(off)
extern char *RwEngineInstance;
#define RS_SET(a, b) (*(void (**)(int, int))(RwEngineInstance + 0x20))(a, b)
extern char g_trail_653258[], g_car_damage_65aa88[];
extern int g_fx_ptank_atomic_a, g_ptank_atomic, g_data_6f6934;
extern void particles_render(void *);
extern void course_sky_atomic_render(void);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x004165d0
void trail_particles_render(void)
{
    RS_SET(8, 0);
    RS_SET(6, 0);
    particles_render(g_trail_653258);
    RS_SET(8, 1);
    RS_SET(6, 1);
}

// FUNCTION: MFL 0x00424690
void car_damage_smoke_render(void)
{
    RS_SET(8, 0);
    particles_render(g_car_damage_65aa88);
    RS_SET(8, 1);
}
