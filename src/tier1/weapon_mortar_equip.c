#include "mfl_tier2.h"
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*006a29b8*/ extern undefined4 g_weapon_6a29b8;
/* Per player record in the globals at 0x6A29B8: 0x1C bytes, 7 undefined4 words. */
#define PLAYER_RECORD_SIZE (0x1c)
/* Value written to record +0xC (the mortar state) by this function (inferred: "equipped"). */
#define MORTAR_STATE_EQUIPPED (3)
/* IEEE 754 bit pattern of -1.0f, written to record +0x10. */
#define FLOAT_MINUS_ONE_BITS (0xbf800000)
/* weapon_mortar_equip */
/* type 0x07; state 0x6A29B8 stride 0x1C */
/*
 * Equips the mortar (weapon type 0x07) into the per player weapon record.
 * slot: the WeaponSlot being equipped; slot->player selects the record.
 * Returns a pointer to that player's record (0x6A29B8 + player_index * 0x1C).
 * Record layout as written here (inferred): +0x0 slot, +0x8 flag set to 1,
 * +0xC mortar state set to MORTAR_STATE_EQUIPPED, +0x10 float -1.0f,
 * +0x14 clump, +0x18 atomic.
 */
undefined4 * weapon_mortar_equip(WeaponSlot *slot)
{
  uint player_index;
  char *rec;

  player_index = slot->player;
  rec = (char *)&g_weapon_6a29b8 + player_index * PLAYER_RECORD_SIZE;
  *(WeaponSlot **)rec = slot;
  *(undefined4 *)(rec + 8) = 1;
  *(undefined4 *)(rec + 0xc) = MORTAR_STATE_EQUIPPED;
  *(undefined4 *)(rec + 0x10) = FLOAT_MINUS_ONE_BITS;
  clump_set_flag_on_atomics(*(RpClump **)(rec + 0x14), 0x1);
  atomic_set_render_flag(*(RpAtomic **)(rec + 0x18),0);
  RpWorldAddClump((RpWorld *)course_get_rpworld_00467e90(),*(RpClump **)(rec + 0x14));
  return (undefined4 *)rec;
}
