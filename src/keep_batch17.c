/* functions of batch17.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch17.c is kept as it was */
#pragma auto_inline(off)
extern char *RwEngineInstance;
#define RW_FREE(p) (*(void (**)(int))(RwEngineInstance + 0x10c))(p)
#define RW_ALLOC(n, f) (*(int (**)(int, int))(RwEngineInstance + 0x108))(n, f)
#define RS_SET(a, b) (*(void (**)(int, int))(RwEngineInstance + 0x20))(a, b)
extern int g_particles_7519b8;
extern int g_fx_atomic_a, g_fx_atomic_b;
extern int g_particles_8869b4[], g_particles_8869cc[], g_particles_8869b8[];
extern int g_particles_7596a4;
extern char g_load_752180[], g_sound_7521c0[];
extern void pool_destroy(int);
extern void particles_destroy(void *);
extern void particles_render(void *);
extern void d3d9_draw_with_transform(void *, int);
extern int g_particles_7519b4;
extern char g_particles_745dc0[];
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00499a40
void fx_particles_render_two(void)
{
    RS_SET(8, 0);
    particles_render(g_sound_7521c0);
    particles_render(g_load_752180);
    RS_SET(8, 1);
}

// FUNCTION: MFL 0x004982d0
void fx_render_additive_overlay(void)
{
    RS_SET(1, 0);
    RS_SET(0xa, 5);
    RS_SET(0xb, 6);
    d3d9_draw_with_transform(g_particles_745dc0, g_particles_7519b4);
    RS_SET(0xa, 5);
    RS_SET(0xb, 6);
}
