#include "mfl_tier2.h"
/*0049a980*/ extern int fx_slot_release();
/*006a1f2c*/ extern char g_course_nt1_6a1f2c[];
/*006a2150*/ extern char g_course_city_6a2150[];
/* course_city_cb5_helper_455aa0 */

/* callers course_city_cb5 */

/* course city callback 5 helper: releases every fx slot held in the g_course_nt1 slot tables and resets them to -1 (unused). */
void course_city_cb5_helper_455aa0(void)

{
  undefined4 *slot;
  
  /* each loop: release the fx slot in every entry of one slot table, then reset it to -1 (unused) */
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0xb4);
  do {
    fx_slot_release(*slot);
    *slot = 0xffffffff;
    slot = slot + 1;
  } while ((char *)slot < ((char *)&g_course_nt1_6a1f2c + 0x11c));
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x64);
  do {
    fx_slot_release(*slot);
    *slot = 0xffffffff;
    slot = slot + 1;
  } while ((char *)slot < ((char *)&g_course_nt1_6a1f2c + 0x94));
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x14);
  do {
    fx_slot_release(*slot);
    *slot = 0xffffffff;
    slot = slot + 1;
  } while ((char *)slot < ((char *)&g_course_nt1_6a1f2c + 0x64));
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x1b4);
  do {
    fx_slot_release(*slot);
    *slot = 0xffffffff;
    slot = slot + 1;
  } while ((char *)slot < ((char *)&g_course_nt1_6a1f2c + 0x20c));
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x218);
  do {
    fx_slot_release(*slot);
    *slot = 0xffffffff;
    slot = slot + 1;
  } while ((char *)slot < ((char *)&g_course_city_6a2150));
  slot = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x98);
  do {
    fx_slot_release(slot[-1]);
    fx_slot_release(*slot);
    slot[-1] = 0xffffffff;
    *slot = 0xffffffff;
    slot = slot + 2;
  } while ((char *)slot < ((char *)&g_course_nt1_6a1f2c + 0xb8));
  return;
}


