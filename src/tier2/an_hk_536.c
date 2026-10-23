#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*0049b780*/ extern int __cdecl particles_alloc_system(struct RwTexture *p0, int p1, int p2, uint p3);
/*0049b860*/ extern int fx_set_blend_modes();
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004f6bf0*/ extern int FUN_004f6bf0(void *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/*006a2a6c*/ extern undefined4 g_mortar_load_6a2a6c;
/*006a4c6c*/ extern char g_explosion_queue_1__position_radius_3_[];
/*006a2a3c*/ extern char g_weapon_6a2a3c[];
#define MORTAR_ATOMIC_COUNT (3) /* atomics per mortar clump (inferred) */
#define WEAPON_SLOT_WORDS (7) /* words per entry of g_weapon_6a29cc (inferred) */
#define MORTAR_SLOT_WORDS (0x44) /* words per entry of g_mortar_load_6a2a6c (inferred) */
#define ATOMIC_PART_MAIN (2) /* FUN_004c52b0 result of the atomic kept in slot[1] (inferred) */
#define BLEND_SRC_ALPHA (5) /* RwBlendFunction rwBLENDSRCALPHA (inferred) */
#define BLEND_INV_SRC_ALPHA (6) /* RwBlendFunction rwBLENDINVSRCALPHA (inferred) */
/* Loads the smoke texture and the mortar models, then fills two slot tables (inferred):
   g_weapon_6a29cc (one entry per weapon slot: cloned mortar clump, render atomic in slot[1])
   and g_mortar_load_6a2a6c (explosion queue entries: round clump, smoke particle system).
   Takes no parameters and returns nothing. */

/* "'smoke' texture, mortar.dff and MortarRound.dff" */

void mortar_load_assets(void)

{
  RwTexture *smoke_texture;
  RpClump *mortar_clump;
  RpClump *mortar_round_clump;
  RpClump *weapon_clump;
  int atomic_part;
  int atomic_index;
  undefined4 *slot;
  undefined4 last_result;
  undefined4 atomics [MORTAR_ATOMIC_COUNT];

  smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
  mortar_clump = (RpClump *)clump_load_by_name("mortar.dff", 0x0, 0x0);
  mortar_round_clump = (RpClump *)clump_load_by_name("MortarRound.dff", 0x0, 0x0);
  slot = &g_weapon_6a29cc;
  /* One pass per weapon slot: clone the mortar clump, clear the slot fields, and set up its atomics (inferred). */
  do {
    weapon_clump = (RpClump *)FUN_004f6bf0(mortar_clump);
    last_result = 0;
    *slot = weapon_clump;
    slot[-3] = 0;
    slot[-1] = 0xbf800000;
    clump_set_flag_on_atomics(weapon_clump, 0x0);
    clump_get_atomics(*slot,atomics);
    atomic_index = 0;
    /* Classify each atomic: negative = ignored; parts 0-1 get render flag 1; ATOMIC_PART_MAIN gets flag 0
       and is kept in slot[1] (inferred). */
    do {
      last_result = atomics[atomic_index];
      atomic_part = FUN_004c52b0(last_result,0,0);
      if (-1 < atomic_part) {
        if (atomic_part < 2) {
          atomic_set_render_flag(last_result,1);
        }
        else if (atomic_part == ATOMIC_PART_MAIN) {
          atomic_set_render_flag(last_result,0);
          slot[1] = last_result;
        }
      }
      atomic_index = atomic_index + 1;
    } while (atomic_index < MORTAR_ATOMIC_COUNT);
    slot = slot + WEAPON_SLOT_WORDS;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_weapon_6a2a3c));
  RpClumpDestroy(mortar_clump);
  slot = &g_mortar_load_6a2a6c;
  /* One pass per explosion queue entry: clone the round clump and start a smoke particle system with blend modes (inferred). */
  do {
    last_result = FUN_004f6bf0(mortar_round_clump);
    slot[1] = last_result;
    slot[-0x10] = 0;
    *slot = 0;
    slot[3] = 0;
    slot[4] = 0;
    last_result = particles_alloc_system(smoke_texture,0xf,2,0x817);
    slot[5] = last_result;
    fx_set_blend_modes(last_result,BLEND_SRC_ALPHA,BLEND_INV_SRC_ALPHA);
    clump_set_flag_on_atomics((RpClump *)slot[1], 0x0);
    slot = slot + MORTAR_SLOT_WORDS;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_explosion_queue_1__position_radius_3_));
  RpClumpDestroy(mortar_round_clump);
  return;
}


