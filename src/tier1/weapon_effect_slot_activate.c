#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004c5440*/ extern int __cdecl FUN_004c5440(int a1, int a2, int a3);
/*006a9424*/ extern undefined4 g_weapon_6a9424;
/*006a9430*/ extern undefined4 g_weapon_6a9430;
/*006a9454*/ extern undefined1 g_weapon_6a9454;
/*006a9455*/ extern undefined1 g_weapon_6a9455;
/*006a9456*/ extern undefined1 g_weapon_6a9456;
/*006a9457*/ extern undefined1 g_weapon_6a9457;
/*006a9474*/ extern undefined4 g_weapon_6a9474;
#define WEAPON_SLOT_RECORD_SIZE (0x58) /* bytes per slot record in g_weapon_6a9454 */
#define WEAPON_SLOT_STRIDE_DWORDS (0x16) /* dwords per slot in the g_weapon_6a9424/6a9430/6a9474 arrays (0x58 / 4) */
#define SLOT_TIMER_START_BITS (0x43340000) /* float 180.0f bit pattern (inferred: timer) */
#define SLOT_EFFECT_FLAG (0x40) /* third argument-flags value for FUN_004c5440 (inferred) */
#define SLOT_COLOUR_RED (0xff) /* record byte 0 (inferred: RGBA) */
#define SLOT_COLOUR_ALPHA (200) /* record byte 3 (inferred: RGBA) */
/* weapon_effect_slot_activate */
/* Activates weapon effect slot `slot`: marks its 0x58 byte record active and enables its clump.
   slot: index of the weapon effect slot. Returns nothing. */
void weapon_effect_slot_activate(int slot)
{
  undefined4 clump_addr;
  int slot_offset;
  RpClump *clump;
  undefined4 enable_flag;

  slot_offset = slot * WEAPON_SLOT_RECORD_SIZE;
  clump = (RpClump *)(&g_weapon_6a9474)[slot * WEAPON_SLOT_STRIDE_DWORDS];
  enable_flag = 1;
  /* mark the slot active (inferred) and start its timer */
  (&g_weapon_6a9430)[slot * WEAPON_SLOT_STRIDE_DWORDS] = 1;
  (&g_weapon_6a9424)[slot * WEAPON_SLOT_STRIDE_DWORDS] = SLOT_TIMER_START_BITS;
  /* set the flag on every atomic of the slot's clump */
  clump_set_flag_on_atomics(clump, 0x1);
  clump_addr = (&g_weapon_6a9474)[slot * WEAPON_SLOT_STRIDE_DWORDS];
  /* write the slot record colour bytes (inferred: RGBA, red and alpha set) */
  (&g_weapon_6a9454)[slot_offset] = SLOT_COLOUR_RED;
  (&g_weapon_6a9455)[slot_offset] = 0;
  (&g_weapon_6a9456)[slot_offset] = 0;
  (&g_weapon_6a9457)[slot_offset] = SLOT_COLOUR_ALPHA;
  FUN_004c5440(clump_addr,SLOT_EFFECT_FLAG,1);
  return;
}
