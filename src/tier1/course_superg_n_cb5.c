#include "mfl_tier2.h"
/*0049a980*/ extern int fx_slot_release();
/*006a2154*/ extern int g_course_superg_6a2154;
/*008c5620*/ extern undefined4 g_course_superg_8c5620;


#define SUPERG_FX_SLOT_TABLE_LEN 0x40   /* entries in g_course_superg_8c5620 */

/* Releases the fx slots in use for the Super G course (count in g_course_superg_6a2154), then marks all table entries free (-1). */
/* Releases the fx slots in use for the Super G course (count in g_course_superg_6a2154),
   then marks all table entries free (-1). */
void course_superg_n_cb5(void)
{
  int i;
  undefined4 *slot_ptr;

  i = 0;
  if (0 < g_course_superg_6a2154) {
    do {
      fx_slot_release((&g_course_superg_8c5620)[i]);
      i = i + 1;
    } while (i < g_course_superg_6a2154);
  }
  slot_ptr = &g_course_superg_8c5620;
  for (i = SUPERG_FX_SLOT_TABLE_LEN; i != 0; i = i + -1) {
    *slot_ptr = 0xffffffff;
    slot_ptr = slot_ptr + 1;
  }
  g_course_superg_6a2154 = 0;
  return;
}
