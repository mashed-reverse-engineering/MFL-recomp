#include "mfl_tier2.h"
/*00495bc0*/ extern int viewport_raster_destroy();
/*0049a980*/ extern int fx_slot_release();
/*004a1490*/ extern int fx_ptank_buffers_destroy();
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a0778*/ extern int g_course_arctic_6a0778;    /* number of fx slots in g_course_arctic_8c5740 */
/*008c5740*/ extern undefined4 g_course_arctic_8c5740;  /* fx slot ids (0x40 entries) */
/*008c5844*/ extern undefined4 g_course_arctic_8c5844;  /* clump pointers, 0x60 bytes (24 entries) */
/*008c5914*/ extern undefined4 g_course_8c5914;         /* clump pointers, 0x0c bytes (3 entries) */
/*008c5924*/ extern undefined4 g_course_8c5924;         /* single clump pointer */
/*008c5920*/ extern char g_camera_8c5920[];
/*008c58a4*/ extern char g_course_arctic_8c58a4[];


/* course_arctic_cb5: course cleanup callback for Arctic; releases fx slots, clumps and the world's clumps. */

void course_arctic_cb5(char *course_base)
{
  int i;
  undefined4 *entry_ptr;

  /* release every fx slot in use */
  i = 0;
  if (0 < g_course_arctic_6a0778) {
    do {
      fx_slot_release((&g_course_arctic_8c5740)[i]);
      i = i + 1;
    } while (i < g_course_arctic_6a0778);
  }
  g_course_arctic_6a0778 = 0;
  /* mark all 0x40 slot entries as invalid (-1) */
  entry_ptr = &g_course_arctic_8c5740;
  for (i = 0x40; i != 0; i = i + -1) {
    *entry_ptr = 0xffffffff;
    entry_ptr = entry_ptr + 1;
  }
  fx_ptank_buffers_destroy();
  /* destroy the 24 clumps of group 8c5844 */
  entry_ptr = &g_course_arctic_8c5844;
  do {
    RpClumpDestroy((RpClump *)*entry_ptr);
    entry_ptr = entry_ptr + 1;
  } while ((int)entry_ptr < ((int)&g_course_arctic_8c58a4));
  viewport_raster_destroy(0);
  /* remove and destroy the 3 clumps of group 8c5914 from the world at course_base+0xc9d4 */
  entry_ptr = &g_course_8c5914;
  do {
    RpWorldRemoveClump(*(RpWorld **)(course_base + 0xc9d4),(RpClump *)*entry_ptr);
    RpClumpDestroy((RpClump *)*entry_ptr);
    entry_ptr = entry_ptr + 1;
  } while ((int)entry_ptr < ((int)&g_camera_8c5920));
  /* same for the single clump 8c5924 */
  RpWorldRemoveClump(*(RpWorld **)(course_base + 0xc9d4),g_course_8c5924);
  RpClumpDestroy(g_course_8c5924);
  return;
}
