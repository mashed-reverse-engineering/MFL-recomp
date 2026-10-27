#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00485ae0*/ extern int particle_templates_init();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5080*/ extern undefined g_particles_6c5080;
/*006c4140*/ extern char g_gfx_6c4140[];


/* smoke_particles_init */

/* "'smoke' texture and particle system" */
/* Sets up the flame and smoke particle systems (ptanks) from the "flames3" and "smoke" textures. */

/* ptank flag words (meaning unknown) */
#define PTANK_FLAGS_SMOKE 0x817
#define PTANK_FLAGS_OTHER 0x807

void smoke_particles_init(void)
{
  RwTexture *texture_dict;
  uint *slot;     /* stride 0x10 uints per ptank */

  particle_templates_init();
  texture_dict = (RwTexture *)find_texture_dict_b("flames3");
  /* first block: 2 ptanks, blend modes 5 and 2 (meaning unknown) */
  slot = (uint *)&g_particles_6c40c0;
  do {
    particle_system_setup_ptank(slot,PTANK_FLAGS_SMOKE,0x40,texture_dict);
    ptank_set_blend_modes(slot,5,2);
    slot = slot + 0x10;
  } while ((int)slot < ((int)&g_gfx_6c4140));
  texture_dict = (RwTexture *)find_texture_dict_b("smoke");
  /* second block: 2 ptanks with the smoke texture */
  slot = (uint *)&g_particles_6c5000;
  do {
    particle_system_setup_ptank(slot,PTANK_FLAGS_SMOKE,0x40,texture_dict);
    slot = slot + 0x10;
  } while ((int)slot < ((int)&g_particles_6c5080));
  /* last ptank: no texture */
  particle_system_setup_ptank((uint *)&g_particles_6c5080,PTANK_FLAGS_OTHER,0x40,(RwTexture *)0x0);
  return;
}
