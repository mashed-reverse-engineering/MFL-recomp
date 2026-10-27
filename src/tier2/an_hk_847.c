#include "mfl_tier2.h"
/*005dd378*/ extern float k_f_0_1;
/*005e64a4*/ extern float k_f_0;
/*0076b830*/ extern undefined g_effects_update_76b830;
/*0076b840*/ extern float g_effects_update_76b840;
/*0076b858*/ extern float g_effects_update_76b858;
/*0076b870*/ extern float g_effects_update_76b870;
/*0076b888*/ extern float g_effects_update_76b888;
/*0076b8a0*/ extern float g_effects_update_76b8a0;
/*0076b8b8*/ extern float g_effects_update_76b8b8;
/*0076b8d0*/ extern float g_effects_update_76b8d0;
/*0076b8e8*/ extern float g_effects_update_76b8e8;
/*0076bec0*/ extern undefined4 g_particles_76bec0;

#define FX_SLOT_COUNT (0x40)
#define FX_SLOT_STRIDE (0x18)

/* fx_floating_text_spawn */

/* Spawns one floating text effect in the FX_SLOT_COUNT-slot table at g_effects_update_76b830 (slots FX_SLOT_STRIDE
   bytes apart). Uses the first free slot. If none is free, it reuses one of the first eight slots: the one whose
   +0x10 field is largest (inferred: an age/timer; the chain of '>' compares keeps the maximum, not the smallest as
   the old text said). Parameters: pos = 3 words copied to slot +0x00..+0x08 (inferred x, y, z); lifetime = stored at
   slot +0x14 (inferred). param_3 and param_4 are unused. Returns nothing.
   (was: "allocates one of 64 slots (or replaces the one with the smallest timer) and starts it with the given value") */

void __cdecl fx_floating_text_spawn(undefined4 *pos,undefined4 lifetime,undefined4 param_3,undefined4 param_4)

{
  float best_timer;
  int slot_index;
  undefined4 *slot;
  uint replace_index;

  best_timer = k_f_0;
  slot_index = 0;
  /* Scan for a free slot (flag in g_particles_76bec0 == 0), claim it and jump to the common setup.
     The break is never reached, because slot is never NULL. */
  do {
    if ((&g_particles_76bec0)[slot_index] == 0) {
      slot = (undefined4 *)((char *)&g_effects_update_76b830 + slot_index * FX_SLOT_STRIDE);
      (&g_particles_76bec0)[slot_index] = 1;
      if (slot != (undefined4 *)0x0) goto LAB_0049db62;
      break;
    }
    slot_index = slot_index + 1;
  } while (slot_index < FX_SLOT_COUNT);
  /* No free slot: pick the slot with the largest +0x10 value among slots 0..7. The compares are unrolled, so
     slots 8..63 are never considered (check whether that is intended). */
  replace_index = 0;
  if (g_effects_update_76b840 > best_timer) {
    replace_index = 0;
    best_timer = g_effects_update_76b840;
  }
  if (g_effects_update_76b858 > best_timer) {
    replace_index = 1;
    best_timer = g_effects_update_76b858;
  }
  if (g_effects_update_76b870 > best_timer) {
    replace_index = 2;
    best_timer = g_effects_update_76b870;
  }
  if (g_effects_update_76b888 > best_timer) {
    replace_index = 3;
    best_timer = g_effects_update_76b888;
  }
  if (g_effects_update_76b8a0 > best_timer) {
    replace_index = 4;
    best_timer = g_effects_update_76b8a0;
  }
  if (g_effects_update_76b8b8 > best_timer) {
    replace_index = 5;
    best_timer = g_effects_update_76b8b8;
  }
  if (g_effects_update_76b8d0 > best_timer) {
    replace_index = 6;
    best_timer = g_effects_update_76b8d0;
  }
  if (g_effects_update_76b8e8 > best_timer) {
    replace_index = 7;
    best_timer = g_effects_update_76b8e8;
  }
  slot = (undefined4 *)((char *)&g_effects_update_76b830 + replace_index * FX_SLOT_STRIDE);
  (&g_particles_76bec0)[replace_index] = 1;
LAB_0049db62:
  /* Common setup for both paths: lifetime at +0x14, zero +0x0c and +0x10 (age/timer reset), copy pos to
     +0x00..+0x08, then raise y (slot[1]) by k_f_0_1 (inferred: floats the text upward). */
  slot[5] = lifetime;
  slot[4] = 0;
  slot[3] = 0;
  *slot = *pos;
  slot[1] = pos[1];
  slot[2] = pos[2];
  *(float *)&slot[1] = *(float *)&slot[1] + k_f_0_1;
  return;
}


