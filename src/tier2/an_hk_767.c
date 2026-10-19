#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0049c0f0*/ extern int fx_system_render_no_zwrite();
/*006a5e34*/ extern undefined g_weapon_rocket_6a5e34;
/*006a6050*/ extern undefined g_weapon_rocket_6a6050;
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define RACE_PHASE_CLEAR_EFFECTS (6) /* (inferred from function name) */
#define OBJECT_METHOD_OFFSET (0x48) /* byte offset of a function pointer inside the object (inferred) */

/* weapon_rocket_clear_effects
 * Per-frame effect pass over the gun and rocket records (purpose inferred from the name and body).
 * Does nothing unless race_get_phase() == RACE_PHASE_CLEAR_EFFECTS.
 * Walks the rocket records from g_weapon_rocket_6a6050 back towards g_weapon_rocket_6a5e34 in steps of
 * 0x6c bytes (5 iterations). Takes no parameters and returns nothing.
 */

void weapon_rocket_clear_effects(void)

{
  int *gun_cooldown;
  int race_phase;
  int *rocket_cursor;
  int *next_rocket_cursor;
  int *gun_clump_cursor;

  race_phase = race_get_phase();
  if (race_phase == RACE_PHASE_CLEAR_EFFECTS) {
    gun_clump_cursor = (int *)&MachinegunState_ARRAY_006a60d0[0].field_0x1c;
    rocket_cursor = (int *)&g_weapon_rocket_6a6050;
    /* Loop over the rocket records, one per iteration (inferred).
       gun_clump_cursor is stepped alongside it through the MachinegunState fields. */
    do {
      gun_cooldown = gun_clump_cursor + -0xc;
      gun_clump_cursor = gun_clump_cursor + -0xb;
      next_rocket_cursor = rocket_cursor + -0x1b;
      /* Cooldown field non-zero (inferred: a shot is pending): call the method on the gun's clump (inferred: per-frame update). */
      if (*gun_cooldown != 0) {
        (**(code **)(*gun_clump_cursor + OBJECT_METHOD_OFFSET))(*gun_clump_cursor);
      }
      /* Rocket record flagged (inferred): if its second flag is also set, call the method on the next record,
         then draw its effect with z-write off (inferred: so the effect is not occluded by later geometry). */
      if (rocket_cursor[-0x20] != 0) {
        if (rocket_cursor[-0x1c] != 0) {
          (**(code **)(*next_rocket_cursor + OBJECT_METHOD_OFFSET))(*next_rocket_cursor);
        }
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
        fx_system_render_no_zwrite(rocket_cursor[-0x1a]);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
      }
      rocket_cursor = next_rocket_cursor;
    } while (next_rocket_cursor != (int *)&g_weapon_rocket_6a5e34);
  }
  return;
}


