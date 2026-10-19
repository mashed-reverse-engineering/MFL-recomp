#include "mfl_tier2.h"
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cc50*/ extern undefined4 g_menu_state_69cc50;
/*0069cc54*/ extern char *g_menu_state_69cc54;
/*0069cc58*/ extern char *g_menu_state_69cc58;
/*0069cc5c*/ extern char *g_menu_state_69cc5c;
/*0069cc8c*/ extern char *g_menu_state_69cc8c;
/*0069d2cc*/ extern undefined4 g_menu_state_69d2cc;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];

#define MENU_RECORD_SIZE (0x40) /* sizeof(struct MenuRecord); the saved copy uses the same stride */
#define MENU_SAVE_END (0x340) /* byte offset where the saved copy stops: 13 records */
#define MENU_ENTRY_SLOTS (12) /* entry_enabled[12] */

/* menu_state_save */

/* "copies the menu table to the saved copy and stores g_menu_current" */
/*
 * Saves the menu state: copies g_menu_current into g_menu_state_69d2cc (the saved current
 * page), then copies records 1..13 of g_menu_records into the saved table at
 * g_menu_state_69cc50, one MENU_RECORD_SIZE block per record. Record 0 is not saved.
 * Takes no parameters, returns nothing.
 */

void menu_state_save(void)

{
  undefined4 field_word; /* temp: holds page_id, then entry_count (inferred) */
  int record_offset; /* byte offset of the record being saved, 0 .. MENU_SAVE_END (inferred) */
  int entries_left; /* countdown over the MENU_ENTRY_SLOTS entry_enabled words */
  undefined4 *src_word; /* walks the live entry_enabled words of the record */
  undefined4 *dst_word; /* walks the saved entry_enabled words of the same record */

  g_menu_state_69d2cc = g_menu_current;
  record_offset = 0;
  /* one pass per record: item_list, page_id, selected, entry_enabled[], entry_count */
  do {
    field_word = *(undefined4 *)((LONG_PTR)g_menu_records[1].entry_enabled + record_offset + -8);
    *(undefined4 *)((char *)&g_menu_state_69cc50 + record_offset) =
         *(undefined4 *)((LONG_PTR)g_menu_records[1].entry_enabled + record_offset + -0xc);
    *(undefined4 *)((char *)&g_menu_state_69cc58 + record_offset) =
         *(undefined4 *)((LONG_PTR)g_menu_records[1].entry_enabled + record_offset + -4);
    *(undefined4 *)((char *)&g_menu_state_69cc54 + record_offset) = field_word;
    field_word = *(undefined4 *)((LONG_PTR)g_menu_records[1].entry_enabled + record_offset + 0x30);
    src_word = (undefined4 *)((LONG_PTR)g_menu_records[1].entry_enabled + record_offset);
    dst_word = (undefined4 *)((char *)&g_menu_state_69cc5c + record_offset);
    /* copy the entry_enabled flags word by word */
    for (entries_left = MENU_ENTRY_SLOTS; entries_left != 0; entries_left = entries_left + -1) {
      *dst_word = *src_word;
      src_word = src_word + 1;
      dst_word = dst_word + 1;
    }
    *(undefined4 *)((char *)&g_menu_state_69cc8c + record_offset) = field_word;
    record_offset = record_offset + MENU_RECORD_SIZE;
  } while (record_offset < MENU_SAVE_END);
  return;
}


