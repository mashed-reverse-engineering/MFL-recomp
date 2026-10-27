#include "mfl_tier2.h"
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cc50*/ extern undefined4 g_menu_state_69cc50;  /* saved menu table, 0x40-byte rows (saved copy of g_menu_records) */
/*0069cc54*/ extern undefined4 g_menu_state_69cc54;  /* saved row field, +4 */
/*0069cc58*/ extern undefined4 g_menu_state_69cc58;  /* saved row field, +8 */
/*0069cc5c*/ extern undefined4 g_menu_state_69cc5c;  /* saved row field, +0xc (12 dwords copied) */
/*0069cc8c*/ extern undefined4 g_menu_state_69cc8c;  /* saved row field, +0x3c from 69cc50 */
/*0069d2cc*/ extern undefined4 g_menu_state_69d2cc;  /* saved value of g_menu_current */
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];

/* menu_state_restore: copies the saved menu table (0x69CC50) back into g_menu_records and restores g_menu_current. */

void menu_state_restore(void)
{
  undefined4 saved_a;
  undefined4 saved_b;
  int row_offset;     /* byte offset of the current 0x40-byte row */
  int dword_count;
  undefined4 *src_ptr;
  undefined4 *dst_ptr;

  g_menu_current = g_menu_state_69d2cc;
  row_offset = 0;
  do {
    /* first few fields of the row are written out of order (as in the original) */
    saved_a = *(undefined4 *)((char *)&g_menu_state_69cc54 + row_offset);
    *(undefined4 *)((int)g_menu_records[1].entry_enabled + row_offset + -0xc) =
         *(undefined4 *)((char *)&g_menu_state_69cc50 + row_offset);
    saved_b = *(undefined4 *)((char *)&g_menu_state_69cc58 + row_offset);
    *(undefined4 *)((int)g_menu_records[1].entry_enabled + row_offset + -8) = saved_a;
    saved_a = *(undefined4 *)((char *)&g_menu_state_69cc8c + row_offset);
    *(undefined4 *)((int)g_menu_records[1].entry_enabled + row_offset + -4) = saved_b;
    *(undefined4 *)((int)g_menu_records[1].entry_enabled + row_offset + 0x30) = saved_a;
    /* remaining 12 dwords of the row, copied one by one */
    src_ptr = (undefined4 *)((char *)&g_menu_state_69cc5c + row_offset);
    dst_ptr = (undefined4 *)((int)g_menu_records[1].entry_enabled + row_offset);
    row_offset = row_offset + 0x40;
    for (dword_count = 0xc; dword_count != 0; dword_count = dword_count + -1) {
      *dst_ptr = *src_ptr;
      src_ptr = src_ptr + 1;
      dst_ptr = dst_ptr + 1;
    }
  } while (row_offset < 0x340);   /* 0x340 = 13 rows of 0x40 */
  return;
}
