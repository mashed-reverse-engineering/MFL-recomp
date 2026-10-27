#include "mfl_tier2.h"
/*0049a980*/ extern int fx_slot_release();
/*006a1f2c*/ extern char g_course_nt1_6a1f2c[];
/*006a2150*/ extern char g_course_city_6a2150[];
/* course_city_cb5_helper_455aa0 */

/* callers course_city_cb5 */

/* course city callback 5 helper: releases every fx slot held in the g_course_nt1 slot tables and resets them to -1 (unused). */
void course_city_cb5_helper_455aa0(void)

{
  undefined4 *slotPtr;
  
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0xb4);
  do {
    fx_slot_release(*slotPtr);
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 1;
  } while ((char *)slotPtr < ((char *)&g_course_nt1_6a1f2c + 0x11c));
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x64);
  do {
    fx_slot_release(*slotPtr);
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 1;
  } while ((char *)slotPtr < ((char *)&g_course_nt1_6a1f2c + 0x94));
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x14);
  do {
    fx_slot_release(*slotPtr);
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 1;
  } while ((char *)slotPtr < ((char *)&g_course_nt1_6a1f2c + 0x64));
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x1b4);
  do {
    fx_slot_release(*slotPtr);
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 1;
  } while ((char *)slotPtr < ((char *)&g_course_nt1_6a1f2c + 0x20c));
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x218);
  do {
    fx_slot_release(*slotPtr);
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 1;
  } while ((char *)slotPtr < ((char *)&g_course_city_6a2150));
  slotPtr = (undefined4 *)((char *)&g_course_nt1_6a1f2c + 0x98);
  do {
    fx_slot_release(slotPtr[-1]);
    fx_slot_release(*slotPtr);
    slotPtr[-1] = 0xffffffff;
    *slotPtr = 0xffffffff;
    slotPtr = slotPtr + 2;
  } while ((char *)slotPtr < ((char *)&g_course_nt1_6a1f2c + 0xb8));
  return;
}


