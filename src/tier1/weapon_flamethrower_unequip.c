#include "mfl_tier2.h"
/*00472bb0*/ extern int sfx_voice_stop();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, int p1);      /* diffemu: the original takes (clump, flag), the flag is read from the stack slot */
/*00484680*/ extern int object_set_flag_4();
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*006a9588*/ extern char *g_weapon_6a9588;
#define POWERUP_STRIDE_FLOATS (0x14)          /* floats per struct Powerup record: 0x50 bytes */
#define WEAPON_FLAME_RECORD_STRIDE (0x68)     /* bytes per player's flame record */
#define SFX_VOICE_FLAMETHROWER (0x16)         /* voice id passed to sfx_play_voice_at (inferred) */
/* layout of *rec in weapon_flamethrower_unequip, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct weapon_flamethrower_unequip_rec_layout {
    undefined4 f_0x0;
    char pad_0x4[28];
    RpClump * f_0x20;
    undefined4 f_0x24;
} weapon_flamethrower_unequip_rec_layout;


/* weapon_flamethrower_unequip */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */

/*
 * Unequips the flamethrower for the player in slot->player: turns off the player's flame clump atomics flag,
 * calls object_set_flag_4 on the player's flame object with flag 0 (inferred), removes the clump from the course
 * world and clears the record's active flag. Parameter: slot (WeaponSlot *). Returns nothing.
 */
/* TIER2: unvalidated 2026-10-29 authority=static */
void weapon_flamethrower_unequip(WeaponSlot *slot)
{
  weapon_flamethrower_unequip_rec_layout *rec;

  rec = (weapon_flamethrower_unequip_rec_layout *)((char *)&g_weapon_6a9588 + slot->player * WEAPON_FLAME_RECORD_STRIDE);
  clump_set_flag_on_atomics(rec->f_0x20,0);
  object_set_flag_4(rec->f_0x24,0);
  RpWorldRemoveClump((RpWorld *)course_get_rpworld_00467e90(),rec->f_0x20);
  rec->f_0x0 = 0;
  sfx_voice_stop(SFX_VOICE_FLAMETHROWER,slot->player);
  return;
}
