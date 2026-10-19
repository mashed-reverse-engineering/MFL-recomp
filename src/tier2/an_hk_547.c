#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004c5440*/ extern int __cdecl FUN_004c5440(int a1, int a2, int a3);
/*006a9424*/ extern undefined4 g_weapon_6a9424;
/*006a9584*/ extern char g_weapon_6a9584[];
#define WEAPON_SLOT_STRIDE (0x16) /* words per slot record: 0x58 bytes */
#define WEAPON_SLOT_CLUMP (0x14) /* word index of the RpClump * in a slot record: byte 0x50 */
/* weapon_slots_6a9424_clear */

/* "clears the weapon slot records from 0x6A9424 and removes their clumps" */

/*
 * Resets every weapon slot record from g_weapon_6a9424 up to g_weapon_6a9584.
 * Takes no parameters and returns nothing. Each record is WEAPON_SLOT_STRIDE
 * words long; its clump pointer is at word WEAPON_SLOT_CLUMP. Per record: clears
 * words 0 and 3, sets words 1 and 2 to 0xffffffff, clears the clump's atomic flag
 * (clump_set_flag_on_atomics, flag 0), sets the byte at +0x30 to 0xff and clears
 * the bytes at +0x31..+0x33, then calls FUN_004c5440 on the clump (inferred: detach).
 */
void weapon_slots_6a9424_clear(void)

{
  undefined4 *weapon_slot;
  RpClump *clump;
  undefined4 unused_zero;

  weapon_slot = &g_weapon_6a9424;
  /* walk the records one stride at a time until the end of the array */
  do {
    clump = (RpClump *)weapon_slot[WEAPON_SLOT_CLUMP];
    unused_zero = 0;
    weapon_slot[3] = 0;
    *weapon_slot = 0;
    weapon_slot[1] = 0xffffffff;
    weapon_slot[2] = 0xffffffff;
    clump_set_flag_on_atomics(clump, 0x0);
    *(undefined1 *)(weapon_slot + 0xc) = 0xff;
    *(undefined1 *)((char *)weapon_slot + 0x31) = 0;
    *(undefined1 *)((char *)weapon_slot + 0x32) = 0;
    *(undefined1 *)((char *)weapon_slot + 0x33) = 0;
    FUN_004c5440(weapon_slot[WEAPON_SLOT_CLUMP],0x40,1);
    weapon_slot = weapon_slot + WEAPON_SLOT_STRIDE;
  } while ((LONG_PTR)weapon_slot < ((LONG_PTR)&g_weapon_6a9584));
  return;
}


