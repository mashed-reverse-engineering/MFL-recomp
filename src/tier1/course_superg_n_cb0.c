#include "mfl_tier2.h"
/*0049a9b0*/ extern int fx_slot_find_free();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*006a2154*/ extern int g_course_superg_6a2154;   /* number of entries stored in g_course_superg_8c5620 so far */
/*008c5620*/ extern undefined4 g_course_superg_8c5620;  /* array of fx slot ids */

/* course_superg_n_cb0: course init callback for Polar Wharf Night; registers the fx slots of three object groups. */

void course_superg_n_cb0(int course_base)
{
  undefined4 slot_id;
  int entry_ptr;   /* current entry, stride 0x40 */
  int i;           /* index within the current group */

  FID_conflict__wprintf("Polar Wharf Night Initialise.\n");
  g_course_superg_6a2154 = 0;
  i = 0;
  /* group 1: count at +0xc000, entries from +0x30 (kind 0xd) */
  if (0 < *(int *)(course_base + 0xc000)) {
    entry_ptr = course_base + 0x30;
    do {
      slot_id = fx_slot_find_free(0xd,entry_ptr,0,0x14);
      (&g_course_superg_8c5620)[g_course_superg_6a2154] = slot_id;
      g_course_superg_6a2154 = g_course_superg_6a2154 + 1;
      i = i + 1;
      entry_ptr = entry_ptr + 0x40;
    } while (i < *(int *)(course_base + 0xc000));
  }
  /* group 2: count at +0xc004, entries from +0xc30 (kind 7) */
  i = 0;
  if (0 < *(int *)(course_base + 0xc004)) {
    entry_ptr = course_base + 0xc30;
    do {
      slot_id = fx_slot_find_free(7,entry_ptr,0,0x14);
      (&g_course_superg_8c5620)[g_course_superg_6a2154] = slot_id;
      g_course_superg_6a2154 = g_course_superg_6a2154 + 1;
      i = i + 1;
      entry_ptr = entry_ptr + 0x40;
    } while (i < *(int *)(course_base + 0xc004));
  }
  /* group 3: count at +0xc008, entries from +0x1830 (kind 0xc) */
  i = 0;
  if (0 < *(int *)(course_base + 0xc008)) {
    entry_ptr = course_base + 0x1830;
    do {
      slot_id = fx_slot_find_free(0xc,entry_ptr,0,0x14);
      (&g_course_superg_8c5620)[g_course_superg_6a2154] = slot_id;
      g_course_superg_6a2154 = g_course_superg_6a2154 + 1;
      i = i + 1;
      entry_ptr = entry_ptr + 0x40;
    } while (i < *(int *)(course_base + 0xc008));
  }
  return;
}
