#include "mfl_tier2.h"
/*0040de10*/ extern int event_log_flush_new();
/*0040e1c0*/ extern int cars_exhaust_and_damage_fx_update();
/*0040e520*/ extern int course_slots_active_count();
/*00414c70*/ extern int trail_render();
/*00416330*/ extern int hud_pickup_icons_update_render();
/*004243e0*/ extern int car_damage_smoke_update();
/*00474380*/ extern int course_falling_objects_update();
/*004857e0*/ extern int particle_pools_update_and_draw();
/*00486170*/ extern int particle_emitters_update_all();
/*00487760*/ extern int effect_sprites_update_all();
/*00493600*/ extern int rwp_objects_cull_and_register_triggers();
/*00497930*/ extern int fx_ptank_particles_update();
/*00498a40*/ extern int fx_trail_batches_update();
/*00499320*/ extern int fx_ring_update();
/*0049a840*/ extern int fx_dust_reset_count();
/*0049a8b0*/ extern int fx_dust_flush();
/*0049ccb0*/ extern int fx_objects_update_all();
/*0049dd40*/ extern int fx_pickup_sparkle_update();
/*004a0260*/ extern int rain_splash_update(void);
/*00652dd0*/ extern undefined4 g_effects_652dd0;
/*00652dd4*/ extern undefined4 g_effects_652dd4;
/*007ef7f4*/ extern float g_tick_seconds;
/* effects_update_frame */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "advances the effects each frame (0x652DD0 frame counter, particles, event log, effects)" */

/*
 * Per-frame update of all visual effects, called once per game frame.
 * Takes no parameters and returns nothing.
 * Does nothing unless g_effects_652dd4 is nonzero (inferred: effects-enabled flag).
 * Increments the frame counter g_effects_652dd0 (inferred), then runs each effect
 * subsystem in the order below. The order matters: updates and draw/cull calls are
 * interleaved, so do not reorder them.
 */
void effects_update_frame(void)

{
  undefined4 ring_update_result;

  /* Gate: skip the whole frame when effects are disabled. */
  if (g_effects_652dd4 != 0) {
    /* Advance the frame counter, then run the dust and event log housekeeping. */
    g_effects_652dd0 = g_effects_652dd0 + 1;
    fx_dust_reset_count();
    event_log_flush_new();
    course_falling_objects_update();
    cars_exhaust_and_damage_fx_update();
    course_slots_active_count();
    fx_trail_batches_update();
    ring_update_result = fx_ring_update();
    rain_splash_update();
    particle_pools_update_and_draw();
    particle_emitters_update_all();
    effect_sprites_update_all();
    fx_pickup_sparkle_update();
    hud_pickup_icons_update_render();
    car_damage_smoke_update();
    fx_objects_update_all();
    rwp_objects_cull_and_register_triggers();
    fx_dust_flush();
    trail_render();
    /* Passes the raw bit pattern of the float tick length, not its value (inferred: callee takes it as int). */
    fx_ptank_particles_update(*(int *)&g_tick_seconds);
  }
  return;
}


