#include "mfl_tier2.h"
/*0049a980*/ extern int fx_slot_release();
/*006a263c*/ extern int g_course_storm_6a263c;
/*008c52b8*/ extern char g_course_storm_8c52b8[];


#define STORM_FX_SLOT_TABLE_LEN 0x40   /* entries in the table at 0x8c52c0 (no extern declared here) */


/* Tears down the Storm course: runs func_0x004ab120, releases the fx slots in use (count in g_course_storm_6a263c), then marks all table entries free (-1). */
/* Tears down the Storm course: runs func_0x004ab120, releases the fx slots in use
   (count in g_course_storm_6a263c), then marks all table entries free (-1). */
void course_storm_cb5(void)
{
  int i;
  undefined4 *slot_ptr;

  func_0x004ab120();
  i = 0;
  if (0 < g_course_storm_6a263c) {
    do {
      fx_slot_release(*(undefined4 *)(i * 4 + ((char *)&g_course_storm_8c52b8 + 0x8)));
      i = i + 1;
    } while (i < g_course_storm_6a263c);
  }
  slot_ptr = (undefined4 *)((char *)&g_course_storm_8c52b8 + 0x8);
  for (i = STORM_FX_SLOT_TABLE_LEN; i != 0; i = i + -1) {
    *slot_ptr = 0xffffffff;
    slot_ptr = slot_ptr + 1;
  }
  g_course_storm_6a263c = 0;
  return;
}
