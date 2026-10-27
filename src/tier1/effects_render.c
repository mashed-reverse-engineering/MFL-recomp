#include "mfl_tier2.h"
/*004146c0*/ extern int trail_clumps_call_update();
/*004165d0*/ extern int trail_particles_render();
/*00424690*/ extern int car_damage_smoke_render();
/*00463270*/ extern int weapon_particles_render();
/*00485900*/ extern int particles_render_all_no_zwrite();
/*00485a20*/ extern int particles_render_all_systems();
/*004877e0*/ extern int effect_sprites_render_all();
/*00498330*/ extern int fx_render();
/*00498d70*/ extern int fx_systems_render();
/*00499a40*/ extern int fx_particles_render_two();
/*0049a930*/ extern int fx_dust_render_0049a930();
/*0049ded0*/ extern int fx_systems_render_all();
/*004a0370*/ extern int clumps_render_all_886024();
/*00652dd0*/ extern undefined4 g_effects_652dd0;
/*00652dd4*/ extern undefined4 g_effects_652dd4;


/* effects_render */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "renders the particle systems when 0x652DD4 is set" */

void effects_render(void)
{
  if (g_effects_652dd4 != 0) {
    fx_dust_render_0049a930();
    weapon_particles_render();
    particles_render_all_no_zwrite();
    fx_particles_render_two();
    fx_systems_render();
    particles_render_all_systems();
    effect_sprites_render_all();
    clumps_render_all_886024();
    fx_systems_render_all();
    trail_particles_render();
    car_damage_smoke_render();
    trail_clumps_call_update();
    fx_render();
    g_effects_652dd0 = 0;
  }
  return;
}
