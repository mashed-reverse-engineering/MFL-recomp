#include "mfl_tier2.h"
/*004685b0*/ extern int voice_queue_select_tables();
/*00468b80*/ extern int voice_queue_remove();
/*008bed04*/ extern undefined4 *g_voice_queue_count_ptr;
/*008bed08*/ extern undefined4 g_voice_queue_entries_ptr;

#define VOICE_QUEUE_TABLE_COUNT (4)
#define VOICE_QUEUE_ENTRY_SIZE (0x28)

/* voice_queue_remove_table_entries */

/* Removes the shared entry list from each of the VOICE_QUEUE_TABLE_COUNT voice queue tables.
 * Takes no parameters and returns nothing.
 * For each table, voice_queue_select_tables(table_index) makes it current, then
 * voice_queue_remove() is called once per entry with the entry's first three dwords.
 * Entries are VOICE_QUEUE_ENTRY_SIZE bytes apart; *g_voice_queue_count_ptr is the
 * last entry index (inferred, -1 = empty), so entries 0..count are processed.
 * (was: "per queue table removes the listed voice ids"; the dwords are not known to be voice ids)
 */

void voice_queue_remove_table_entries(void)

{
  int table_index;
  int entry_offset;
  int entry_index;

  /* For each table: select it, then remove its entries. */
  table_index = 0;
  do {
    voice_queue_select_tables(table_index);
    entry_index = 0;
    if (-1 < (int)*g_voice_queue_count_ptr) {
      entry_offset = 0;
      /* Walk the entry list. The test is <= count, so the loop body runs count + 1 times (inferred). */
      do {
        voice_queue_remove(*(undefined4 *)(entry_offset + g_voice_queue_entries_ptr),
                           *(undefined4 *)(entry_offset + 4 + g_voice_queue_entries_ptr),
                           *(undefined4 *)(entry_offset + 8 + g_voice_queue_entries_ptr));
        entry_index = entry_index + 1;
        entry_offset = entry_offset + VOICE_QUEUE_ENTRY_SIZE;
      } while (entry_index <= (int)*g_voice_queue_count_ptr);
    }
    table_index = table_index + 1;
  } while (table_index < VOICE_QUEUE_TABLE_COUNT);
  return;
}


