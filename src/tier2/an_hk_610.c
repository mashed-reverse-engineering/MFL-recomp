#include "mfl_tier2.h"
extern char s_33s_33s_33s__0062aa58[];
extern char s_fff_fff_fff__0062ab1c[];
/*005dd338*/ extern float k_f_0_01;
/*0062a980*/ extern undefined g_fx_slot_62a980;
/*0062a990*/ extern undefined g_fx_slot_62a990;
/*0062a9a0*/ extern undefined g_fx_slot_62a9a0;
/*0062a9c4*/ extern undefined g_fx_slot_62a9c4;
/*0062a9e4*/ extern undefined g_fx_slot_62a9e4;
/*0062a9f4*/ extern undefined g_fx_slot_62a9f4;
/*0062aa04*/ extern undefined g_fx_slot_62aa04;
/*0062aa28*/ extern undefined g_fx_slot_62aa28;
/*0062aa48*/ extern undefined g_fx_slot_62aa48;
/*0062aa68*/ extern undefined g_fx_slot_62aa68;
/*0062aa88*/ extern undefined g_fx_slot_62aa88;
/*0062aaa8*/ extern undefined g_fx_slot_62aaa8;
/*0062aab8*/ extern undefined g_fx_slot_62aab8;
/*0062aac8*/ extern undefined g_fx_slot_62aac8;
/*0062aaec*/ extern undefined g_fx_slot_62aaec;
/*0062ab0c*/ extern undefined g_fx_slot_62ab0c;
/*0062ab2c*/ extern undefined g_fx_slot_62ab2c;
/*0062ab4c*/ extern undefined g_fx_slot_62ab4c;
/*0062ab6c*/ extern undefined g_fx_slot_62ab6c;
/*0062ab7c*/ extern undefined g_fx_slot_62ab7c;
/*0062ab8c*/ extern undefined g_fx_slot_62ab8c;
/*0062abb0*/ extern undefined g_fx_slot_62abb0;
/*007596a4*/ extern undefined4 g_particles_7596a4;
/*007596a8*/ extern undefined g_fx_slot_7596a8;
/*007596cc*/ extern undefined g_fx_slot_7596cc;
/*008869a0*/ extern undefined g_particles_8869a0;
/*008869b4*/ extern undefined4 g_particles_8869b4;
/* words per particle slot (56 bytes) */
#define FX_SLOT_WORDS (0xe)
/* word index of the in-use flag in a slot (+0x14) */
#define FX_SLOT_IN_USE (5)
/* first address past the end of the slot pool (inferred) */
#define FX_SLOT_POOL_END (0x88859f)
/* fx_slot_find_free
 *
 * Claims the first free slot in the particle/effect pool (g_particles_8869a0),
 * fills it in for effect type fx_type, and returns the slot index.
 * position: pointer to 3 words (x, y, z) copied into slot words 1-3.
 * owner_id: stored in slot word 11 (+0x2c) (inferred).
 * percent: strength 0-100 for types 4 and 6, stored as a 0..1 fraction in word 12 (+0x30) (inferred).
 * Returns the slot index, or -1 when the pool is full.
 */

/* "first slot with +0x14 == 0" */

int fx_slot_find_free(undefined4 fx_type,undefined4 *position,undefined4 owner_id,int percent)

{
  float fraction;
  int slot_index;
  undefined4 *slot;

  slot_index = 0;
  slot = (undefined4 *)&g_particles_8869a0;
  /* walk the pool until a slot whose in-use flag (+0x14) is 0; -1 if none */
  while (slot[FX_SLOT_IN_USE] != 0) {
    slot = slot + FX_SLOT_WORDS;
    slot_index = slot_index + 1;
    if (FX_SLOT_POOL_END < (LONG_PTR)slot) {
      return -1;
    }
  }
  slot[1] = *position;
  slot[2] = position[1];
  slot[3] = position[2];
  slot[0xb] = owner_id;
  *slot = fx_type;
  slot[0xc] = 0;
  /* per-type setup: word 4 is a count/type (inferred), words 7-10 are pointers to
     the effect's global data (inferred), word 6 is a float (-0.5, -1.25 or +0.5 by type) */
  switch(fx_type) {
  case 0:
    slot[4] = 5;
    slot[7] = &g_fx_slot_62a990;
    slot[8] = &g_fx_slot_62a980;
    slot[10] = &g_fx_slot_62a9c4;
    slot[9] = &g_fx_slot_62a9a0;
    slot[6] = 0xbf000000;
    goto switchD_0049aa06_caseD_8;
  case 1:
    slot[7] = &g_fx_slot_62a9f4;
    slot[8] = &g_fx_slot_62a9e4;
    slot[10] = &g_fx_slot_62aa28;
    slot[9] = &g_fx_slot_62aa04;
    goto LAB_0049aba7;
  case 2:
    slot[4] = 1;
    slot[7] = s_33s_33s_33s__0062aa58;
    slot[8] = &g_fx_slot_62aa48;
    slot[10] = &g_fx_slot_62aa68;
    slot[9] = &g_fx_slot_7596a8;
    break;
  case 3:
    slot[7] = s_33s_33s_33s__0062aa58;
    slot[8] = &g_fx_slot_62aa48;
    slot[10] = &g_fx_slot_62aa88;
    slot[9] = &g_fx_slot_7596a8;
    goto LAB_0049aba7;
  case 4:
    slot[4] = 3;
    slot[7] = &g_fx_slot_62aab8;
    slot[8] = &g_fx_slot_62aaa8;
    fraction = (float)percent * k_f_0_01;
    slot[10] = &g_fx_slot_62aaec;
    slot[9] = &g_fx_slot_62aac8;
    *(float *)&slot[0xc] = fraction;
    break;
  case 5:
    slot[4] = 5;
    slot[7] = &g_fx_slot_62a9f4;
    slot[8] = &g_fx_slot_62a9e4;
    slot[10] = &g_fx_slot_62aa28;
    slot[9] = &g_fx_slot_62aa04;
    break;
  case 6:
    slot[4] = 3;
    slot[7] = &g_fx_slot_62aab8;
    slot[8] = &g_fx_slot_62aaa8;
    fraction = (float)percent * k_f_0_01;
    slot[10] = &g_fx_slot_62aa68;
    slot[9] = &g_fx_slot_62aac8;
    *(float *)&slot[0xc] = fraction;
    break;
  case 7:
    slot[4] = 3;
    slot[7] = s_fff_fff_fff__0062ab1c;
    slot[8] = &g_fx_slot_62ab0c;
    slot[10] = &g_fx_slot_62aaec;
    slot[9] = &g_fx_slot_7596cc;
    break;
  default:
    goto switchD_0049aa06_caseD_8;
  case 0xb:
    slot[4] = 3;
    slot[7] = &g_fx_slot_62ab7c;
    slot[8] = &g_fx_slot_62ab6c;
    slot[10] = &g_fx_slot_62abb0;
    slot[9] = &g_fx_slot_62ab8c;
    slot[6] = 0xbfa00000;
    goto switchD_0049aa06_caseD_8;
  case 0xc:
    slot[10] = &g_fx_slot_62ab2c;
    goto LAB_0049ab92;
  case 0xd:
    slot[10] = &g_fx_slot_62ab4c;
LAB_0049ab92:
    slot[7] = s_fff_fff_fff__0062ab1c;
    slot[8] = &g_fx_slot_62ab0c;
    slot[9] = &g_fx_slot_7596cc;
LAB_0049aba7:
    slot[4] = 2;
  }
  /* common default for word 6 (+0.5) when the type did not set it */
  slot[6] = 0x3f000000;
switchD_0049aa06_caseD_8:
  /* mark this slot in use (+0x14 = 1); g_particles_7596a4 is reset (purpose unknown) */
  (&g_particles_8869b4)[slot_index * FX_SLOT_WORDS] = 1;
  g_particles_7596a4 = 0;
  return slot_index;
}


