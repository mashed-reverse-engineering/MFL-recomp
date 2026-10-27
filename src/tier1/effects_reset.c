#include "mfl_tier2.h"
/*00414f00*/ extern int trail_update_00414f00();
/*004165a0*/ extern int trail_reset();
/*004241b0*/ extern int car_damage_smoke_reset();
/*00485610*/ extern int particle_pool_counts_reset();
/*00486160*/ extern int particle_templates_init_b();
/*00487620*/ extern int sprite_slots_clear();
/*00498250*/ extern int fx_tables_clear();
/*00498dc0*/ extern int effects_reset_four_systems();
/*00499a80*/ extern int fx_ring_clear();
/*0049a840*/ extern int fx_dust_reset_count();
/*0049cb00*/ extern int fx_objects_reset_all();
/*0049df50*/ extern int fx_slots_clear_8();
/*0049f6d0*/ extern int fx_styles_init_all();
/*004a03b0*/ extern int fx_clump_slots_clear();
/*00652dc0*/ extern undefined4 g_effects_reset_652dc0;
/*00652dc4*/ extern undefined4 g_effects_reset_652dc4;
/*00652dc8*/ extern undefined4 g_effects_reset_652dc8;
/*00652dcc*/ extern undefined4 g_car_652dcc;


/* effects_reset */

/* "resets the particle systems between races" */

void effects_reset(void)
{
  fx_styles_init_all();
  trail_update_00414f00();
  fx_clump_slots_clear();
  particle_pool_counts_reset();
  particle_templates_init_b();
  sprite_slots_clear();
  fx_dust_reset_count();
  fx_ring_clear();
  effects_reset_four_systems();
  fx_objects_reset_all();
  fx_slots_clear_8();
  trail_reset();
  fx_tables_clear();
  car_damage_smoke_reset();
  g_effects_reset_652dc0 = 0;
  g_effects_reset_652dc4 = 0;
  g_effects_reset_652dc8 = 0;
  g_car_652dcc = 0;
  return;
}
