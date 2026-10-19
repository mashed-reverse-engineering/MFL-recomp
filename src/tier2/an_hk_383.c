#include "mfl_tier2.h"
/*00468450*/ extern int array_find_index();
/*00468480*/ extern int __cdecl sound_find_active_voice_for_slot(undefined4 p0, undefined4 p1);
/*004684c0*/ extern int __cdecl weapon_stack_collect_types(undefined4 p0, undefined4 p1);
/*004685b0*/ extern int voice_queue_select_tables();
/*00468a60*/ extern int __cdecl players_same_team_state(undefined4 p0, undefined4 p1);
/*00610dd8*/ extern char *g_sound_610dd8;
/*008bed04*/ extern undefined4 *g_voice_queue_count_ptr;
/*008bed08*/ extern undefined4 g_voice_queue_entries_ptr;

#define MAX_VOICE_QUEUE_ENTRIES (0x20)
/* bytes per queue entry (inferred: the sound table entry copied into it has the same stride) */
#define VOICE_ENTRY_SIZE (0x28)
#define NOT_FOUND (-1)

/* voice_queue_add */

/* "(line, team, player): adds a voice line to the team queue unless it is already queued / playing
   or the pair is on the same team (max 0x20 entries)" */

typedef struct { undefined4 d[10]; } Dw10;
void voice_queue_add(int line,undefined4 team,undefined4 player)

{
  int queue_entries;
  int queue_count;
  int found_index;
  undefined4 *src_entry;
  undefined4 *dst_entry;

  queue_count = players_same_team_state(team,player);
  if (queue_count == 0) {
    voice_queue_select_tables(team);
    queue_count = *g_voice_queue_count_ptr;
    /* queue only if there is room and the line is neither already active nor already queued */
    if ((((queue_count < MAX_VOICE_QUEUE_ENTRIES) &&
         (queue_entries = g_voice_queue_entries_ptr, found_index = sound_find_active_voice_for_slot(line,queue_entries),
         found_index != 1)) &&
        (found_index = weapon_stack_collect_types(line,queue_entries), found_index != 1)) &&
       (found_index = array_find_index(line,queue_entries,queue_count), found_index == NOT_FOUND)) {
      src_entry = (undefined4 *)((char *)&g_sound_610dd8 + line * VOICE_ENTRY_SIZE);
      dst_entry = (undefined4 *)(queue_entries + queue_count * VOICE_ENTRY_SIZE);
      /* copy the 10-dword sound table entry for this line into the new queue slot (inferred) */
      *(Dw10 *)dst_entry = *(Dw10 *)src_entry;
      *(undefined4 *)(g_voice_queue_entries_ptr + 8 + *g_voice_queue_count_ptr * VOICE_ENTRY_SIZE) = player;
      *(undefined4 *)(g_voice_queue_entries_ptr + 4 + *g_voice_queue_count_ptr * VOICE_ENTRY_SIZE) = team;
      *g_voice_queue_count_ptr = *g_voice_queue_count_ptr + 1;
    }
  }
  return;
}


