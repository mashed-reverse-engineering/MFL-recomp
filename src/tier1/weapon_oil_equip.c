#include "mfl_tier2.h"
/*00467e90*/ extern void * course_get_rpworld_00467e90();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, int p1);      /* diffemu: the original takes (clump, flag), the flag is read from the stack slot */
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*006a7c78*/ extern char *g_weapon_6a7c78;
#define OIL_RECORD_BYTES (0x10)         /* the same stride in bytes */
#define OIL_FULL_AMMO_BITS (0x3f800000) /* float 1.0 bits written to the record on equip: full ammo (inferred) */
/* weapon_oil_free_assets */
/* weapon_oil_equip */
/* type 0x13; state 0x6A7C78 stride 0x10 */
/* Equips the oil weapon for the player that owns weapon slot (returns the player's oil record, undefined4 *).
 * The record is indexed by slot->player (stride OIL_RECORD_BYTES): the slot pointer is stored in it,
 * its first dword is set to full ammo (1.0f, inferred), its clump is added to the course world, the
 * clump's atomics get flag 1, and the record's active dword is set (inferred). */
undefined4 * weapon_oil_equip(WeaponSlot *slot)
{
  char *rec;

  rec = (char *)&g_weapon_6a7c78 + slot->player * OIL_RECORD_BYTES;
  *(WeaponSlot **)(rec + 8) = slot;
  *(undefined4 *)rec = OIL_FULL_AMMO_BITS;
  RpWorldAddClump((RpWorld *)course_get_rpworld_00467e90(),*(RpClump **)(rec + 0xc));
  clump_set_flag_on_atomics(*(RpClump **)(rec + 0xc),1);
  *(undefined4 *)(rec + 4) = 1;
  return (undefined4 *)rec;
}
