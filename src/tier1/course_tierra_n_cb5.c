#include "mfl_tier2.h"
/*0049a980*/ extern int fx_slot_release();
/*004a5da0*/ extern int object_destroy_frames();
/*006a2654*/ extern int g_course_tierra_6a2654;
/*006a2658*/ extern undefined4 g_course_tierra_6a2658;
/*006a265c*/ extern undefined4 g_course_tierra_6a265c;
/*006a2660*/ extern undefined4 g_course_tierra_6a2660;
/*006a2664*/ extern undefined4 g_course_tierra_6a2664;
/*008c4b20*/ extern undefined4 g_course_tierra_8c4b20;


#define TIERRA_FX_SLOT_TABLE_LEN 0x40   /* entries in g_course_tierra_8c4b20 */

/* Tears down the Tierra course: destroys its four object frames, releases the fx slots in use (count in g_course_tierra_6a2654), then marks all table entries free (-1). */
/* Tears down the Tierra course: destroys its four object frames, releases the
   fx slots in use (count in g_course_tierra_6a2654), then marks all table entries free (-1). */
void course_tierra_n_cb5(void)
{
  int i;
  undefined4 *slot_ptr;

  func_0x004575a0();
  object_destroy_frames(g_course_tierra_6a2658);
  object_destroy_frames(g_course_tierra_6a265c);
  object_destroy_frames(g_course_tierra_6a2660);
  object_destroy_frames(g_course_tierra_6a2664);
  i = 0;
  if (0 < g_course_tierra_6a2654) {
    do {
      fx_slot_release((&g_course_tierra_8c4b20)[i]);
      i = i + 1;
    } while (i < g_course_tierra_6a2654);
  }
  slot_ptr = &g_course_tierra_8c4b20;
  for (i = TIERRA_FX_SLOT_TABLE_LEN; i != 0; i = i + -1) {
    *slot_ptr = 0xffffffff;
    slot_ptr = slot_ptr + 1;
  }
  g_course_tierra_6a2654 = 0;
  return;
}
