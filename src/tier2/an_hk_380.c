#include "mfl_tier2.h"
extern char PTR_weapon_machinegun_clear_effects_0060dc40[];
/*00464940*/ extern int powerups_render();
/*00465f80*/ extern int weapon_slots_render_all();
/*0060de60*/ extern undefined4 weapon_descriptor_count;

/* Pointer slots per weapon descriptor entry in the table (inferred). */
#define WEAPON_DESCRIPTOR_STRIDE (0x10)

/* weapons_effects_render_all */

/* "effects render, powerups_render, then every weapon descriptor's render hook" */

void weapons_effects_render_all(void)

{
  int i;
  undefined **descriptor;

  weapon_slots_render_all();
  powerups_render();
  i = 0;
  /* Call each weapon descriptor's render hook (first slot of its entry) in table order. */
  if (0 < (int)weapon_descriptor_count) {
    descriptor = &PTR_weapon_machinegun_clear_effects_0060dc40;
    do {
      (*(code *)*descriptor)();
      i = i + 1;
      descriptor = descriptor + WEAPON_DESCRIPTOR_STRIDE;
    } while (i < (int)weapon_descriptor_count);
  }
  return;
}


