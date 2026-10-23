#include "mfl_tier2.h"
/*004685b0*/ extern int voice_queue_select_tables();
/*00468b80*/ extern int voice_queue_remove();
/*005ddc28*/ extern float k_f_0;
/*005df934*/ extern float k_f_0_0166;
/*008bed04*/ extern undefined4 *g_voice_queue_count_ptr;
/*008bed08*/ extern undefined4 g_voice_queue_entries_ptr;

#define VOICE_QUEUE_TABLE_COUNT (4)
#define VOICE_QUEUE_ENTRY_SIZE (0x28)
#define VOICE_QUEUE_ENTRY_DURATION (0xc) /* (inferred) float, compared against k_f_0 and against elapsed */
#define VOICE_QUEUE_ENTRY_ELAPSED (0x10) /* (inferred) float, seconds played so far */

/* voice_queue_advance_timers
 *
 * Advances the voice queue timers by one frame. For each of the VOICE_QUEUE_TABLE_COUNT tables it selects the table,
 * then walks its entries. An entry with a positive duration (k_f_0 < duration) has k_f_0_0166 (1/60 s, inferred from the
 * name) added to its elapsed time. Once elapsed exceeds duration, the entry is removed with voice_queue_remove(), using
 * the entry's first three fields as arguments.
 * Parameters: none. Returns nothing.
 */

/* "for each queue table adds 1/60 to the elapsed time of entries with a duration" */

void voice_queue_advance_timers(void)

{
  int entries;
  int table_index;
  char *entry_offset;
  int entry_index;

  table_index = 0;
  /* for each queue table: advance the timers of its entries */
  do {
    voice_queue_select_tables(table_index);
    entry_index = 0;
    if (0 < (int)*g_voice_queue_count_ptr) {
      entry_offset = 0;
      entries = g_voice_queue_entries_ptr;
      /* for each entry of the table; the stride is VOICE_QUEUE_ENTRY_SIZE bytes */
      do {
        /* entries with a duration: add one frame to elapsed time */
        if (k_f_0 < *(float *)(entry_offset + VOICE_QUEUE_ENTRY_DURATION + entries)) {
          *(float *)(entry_offset + VOICE_QUEUE_ENTRY_ELAPSED + entries) =
               *(float *)(entry_offset + VOICE_QUEUE_ENTRY_ELAPSED + entries) + k_f_0_0166;
          entries = g_voice_queue_entries_ptr;
          /* elapsed has passed the duration: remove the entry (the next entry shifts into this slot, inferred) */
          if (*(float *)(entry_offset + VOICE_QUEUE_ENTRY_DURATION + g_voice_queue_entries_ptr) <
              *(float *)(entry_offset + VOICE_QUEUE_ENTRY_ELAPSED + g_voice_queue_entries_ptr)) {
            voice_queue_remove(*(undefined4 *)(entry_offset + g_voice_queue_entries_ptr),
                               *(undefined4 *)(entry_offset + 4 + g_voice_queue_entries_ptr),
                               *(undefined4 *)(entry_offset + 8 + g_voice_queue_entries_ptr));
            entries = g_voice_queue_entries_ptr;
          }
        }
        entry_index = entry_index + 1;
        entry_offset = entry_offset + VOICE_QUEUE_ENTRY_SIZE;
      } while (entry_index < (int)*g_voice_queue_count_ptr);
    }
    table_index = table_index + 1;
  } while (table_index < VOICE_QUEUE_TABLE_COUNT);
  return;
}


