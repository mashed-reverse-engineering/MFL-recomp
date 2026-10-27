#include "mfl_tier2.h"
/*00468450*/ extern int array_find_index();
/*004685b0*/ extern int voice_queue_select_tables();
/*008bed04*/ extern int *g_voice_queue_count_ptr;
/*008bed08*/ extern char *g_voice_queue_entries_ptr;
typedef struct { undefined4 d[10]; } VoiceEntry;      /* queue entry, 0x28 bytes */
/* voice_queue_remove */

/* "(line, team): removes a line from the team queue, shifting the following 0x28 byte entries" */

void voice_queue_remove(undefined4 line, undefined4 team, undefined4 unused_3)

{
  int found_index;
  int iVar1;
  int *count_ptr;
  
  voice_queue_select_tables(team);
  count_ptr = g_voice_queue_count_ptr;
  iVar1 = *g_voice_queue_count_ptr;
  found_index = array_find_index(line,g_voice_queue_entries_ptr,iVar1);
  if (found_index != -1) {
    if (found_index < iVar1 + -1) {
      iVar1 = found_index * 0x28;
      do {
        *(VoiceEntry *)(iVar1 + g_voice_queue_entries_ptr) = *(VoiceEntry *)(iVar1 + g_voice_queue_entries_ptr + 0x28);
        found_index = found_index + 1;
        iVar1 = iVar1 + 0x28;
        count_ptr = g_voice_queue_count_ptr;
      } while (found_index < *g_voice_queue_count_ptr + -1);
    }
    *count_ptr = *count_ptr + -1;
  }
  return;
}


