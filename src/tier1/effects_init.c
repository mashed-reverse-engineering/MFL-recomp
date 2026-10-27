#include "mfl_tier2.h"
/*00416230*/ extern int trail_particles_destroy();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00485960*/ extern int particles_destroy_all();
/*00485a90*/ extern int particle_systems_destroy_all();
/*004878f0*/ extern int sprite_slots_destroy();
/*00498370*/ extern int fx_atomics_destroy();
/*00498960*/ extern int fx_systems_destroy();
/*00498e20*/ extern int fx_particles_destroy_two();
/*00499c80*/ extern int fx_dust_destroy();
/*0049d350*/ extern int fx_systems_destroy_all();
/*0049dff0*/ extern int fx_texture_destroy();
/*0049feb0*/ extern int fx_clumps_destroy();
/*0050b5e0*/ extern int nop_stub();
/*00652dd8*/ extern undefined4 g_load_652dd8;
/*00652ddc*/ extern undefined4 g_effects_652ddc;
/*00652de0*/ extern undefined4 g_effects_652de0;
/*00652de4*/ extern undefined4 g_effects_652de4;


/* effects_init */

/* "initialises the particle / effect subsystems (many particle systems)" */

void effects_init(void)
{
  fx_texture_destroy();
  fx_clumps_destroy();
  sprite_slots_destroy();
  particle_systems_destroy_all();
  particles_destroy_all();
  nop_stub();
  fx_systems_destroy_all();
  fx_dust_destroy();
  fx_systems_destroy();
  fx_particles_destroy_two();
  trail_particles_destroy();
  fx_atomics_destroy();
  RwTexDictionaryDestroy_004d5ac0(g_load_652dd8);
  RwTexDictionaryDestroy_004d5ac0(g_effects_652ddc);
  RwTexDictionaryDestroy_004d5ac0(g_effects_652de0);
  RwTexDictionaryDestroy_004d5ac0(g_effects_652de4);
  return;
}
