#include "mfl_tier2.h"
/*00751a18*/ extern undefined4 g_effects_update_751a18;
/*00751b38*/ extern undefined4 g_effects_update_751b38;
/*00751b54*/ extern char *g_effects_update_751b54;
/*00751b70*/ extern char *g_effects_update_751b70;
/*00751b8c*/ extern char *g_effects_update_751b8c;
/*00751ba8*/ extern char *g_effects_update_751ba8;
/* Effect slots per effect system (inferred from the index stride). */
#define SLOTS_PER_SYSTEM (0x7a)
/* An explosion is drawn as this many sub puffs. */
#define SUB_PUFFS_PER_SPAWN (4)
/* Dwords of position data per slot: SUB_PUFFS_PER_SPAWN puffs x 3 floats (inferred). */
#define PUFF_DWORDS_PER_SLOT (0xc)
static undefined4 vehicle_part_position_by_index_r(int reg_eax, int reg_ecx, int reg_esi);
extern void vehicle_part_position_by_index(void);
__declspec(naked) undefined4 vehicle_part_position_by_index_r(int reg_eax, int reg_ecx, int reg_esi)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call vehicle_part_position_by_index
        pop esi
        ret
    }
}

/* fx_explosion_spawn */

/* "spawns an explosion in system system_id slot slot_id (4 sub puffs)" */

/*
 * Spawns an explosion effect in slot `slot_id` of effect system `system_id`.
 * Does nothing if that slot is already active (flag 751b38 == 1).
 * Otherwise:
 *   - copies the slot's world position (from vehicle_part_position_by_index)
 *     into the SUB_PUFFS_PER_SPAWN puff position entries of the slot,
 *   - marks the slot active and records its slot_id,
 *   - clears the per-slot value at 751b54 and stores param_3 / param_4 into
 *     the per-slot tables at 751b70 / 751b8c (meaning inferred, not confirmed).
 */
void fx_explosion_spawn(int system_id,int slot_id,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puff_ptr;
  int puffs_left;
  int slot_index;
  int slot_byte_offset;
  struct {
    undefined4 slot_pos_x;
    undefined4 slot_pos_y;
    undefined4 slot_pos_z;
  } frame;

  slot_index = system_id * SLOTS_PER_SYSTEM + slot_id;
  slot_byte_offset = slot_index * 4;
  if ((&g_effects_update_751b38)[slot_index] != 1) {
    /* read the slot's position into frame (3 dwords) */
    vehicle_part_position_by_index_r(slot_id, system_id, &frame.slot_pos_x);
    /* copy the position into every sub puff, walking backwards through the slot's block */
    puffs_left = SUB_PUFFS_PER_SPAWN;
    puff_ptr = &g_effects_update_751a18 + system_id * SLOTS_PER_SYSTEM + slot_id * PUFF_DWORDS_PER_SLOT;
    do {
      puffs_left = puffs_left + -1;
      puff_ptr[-3] = frame.slot_pos_x;
      puff_ptr[-2] = frame.slot_pos_y;
      puff_ptr[-1] = frame.slot_pos_z;
      puff_ptr = puff_ptr + -3;
    } while (puffs_left != 0);
    /* mark the slot active and record its id */
    *(int *)((char *)&g_effects_update_751ba8 + slot_byte_offset) = slot_id;
    (&g_effects_update_751b38)[slot_index] = 1;
    /* reset the per-slot value and store the caller's values */
    *(undefined4 *)((char *)&g_effects_update_751b54 + slot_byte_offset) = 0;
    *(undefined4 *)((char *)&g_effects_update_751b70 + slot_byte_offset) = param_3;
    *(undefined4 *)((char *)&g_effects_update_751b8c + slot_byte_offset) = param_4;
  }
  return;
}


