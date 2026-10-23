#include "mfl_tier2.h"
/*00422050*/ extern int vehicle_get_object_field_80();
/*00422400*/ extern int __cdecl vehicle_get_active_flag(undefined4 p0);
/*00434bb0*/ extern int get_event_type();
/*004379a0*/ extern int menu_event_course_index_get();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0240*/ extern char g_ai_editor_view_origin_x[];

/* record value = group * RECORD_GROUP_STRIDE + team (inferred) */
#define RECORD_GROUP_STRIDE (6)
/* race_course_record_for_event */

/* cdecl, two stack arguments (table, index); the decompiler's reg_edx parameter does not exist
 * (edx is only a scratch register in the original) */
/* Checks the four player slots against the course record for the current event and
 * returns 1 if any slot is inactive or its object_field_80 differs from
 * team + group * RECORD_GROUP_STRIDE, 0 otherwise.
 * record_table: array of pointers to course entries; the word at +4 of each entry is the record value.
 * entry_index: index into record_table (inferred from the call sites).
 * The record value is replaced by the menu's course index for events 0xB..0xD and 6..8, and the
 * group is forced to 12 for event 0xE (inferred).
 * (was: "selects the course record index for events 0xB..0xD from ui_get_menu_69cfb0") */

undefined4 __cdecl race_course_record_for_event(char *record_table,int entry_index)

{
  int event_or_team;
  int record_value;
  int call_result;
  int slot;
  int *team_ptr;
  undefined4 mismatch_found;

  record_value = *(int *)(*(char **)(record_table + 4 + entry_index * 4) + 4);
  slot = 0;
  mismatch_found = 0;
  /* events 0xB, 0xC, 0xD: the course comes from the menu, not from entry_index */
  event_or_team = get_event_type();
  if (event_or_team == EVENT_QUICK_RACE_11) {
LAB_0040e7a8:
    record_value = menu_event_course_index_get();
    record_value = *(int *)(*(char **)(record_table + 4 + record_value * 4) + 4);
  }
  else {
    event_or_team = get_event_type();
    if (event_or_team == EVENT_QUICK_RACE_12) goto LAB_0040e7a8;
    event_or_team = get_event_type();
    if (event_or_team == EVENT_QUICK_RACE_13) goto LAB_0040e7a8;
  }
  /* events 6, 7, 8: the course comes from the menu as well */
  event_or_team = get_event_type();
  if (event_or_team != EVENT_MULTIPLAYER_6) {
    event_or_team = get_event_type();
    if (event_or_team != EVENT_MULTIPLAYER_7) {
      event_or_team = get_event_type();
      if (event_or_team != EVENT_MULTIPLAYER_8) goto LAB_0040e7fe;
    }
  }
  record_value = menu_event_course_index_get();
  record_value = *(int *)(*(char **)(record_table + 4 + record_value * 4) + 4);
LAB_0040e7fe:
  /* record value -> group number */
  record_value = record_value / RECORD_GROUP_STRIDE;
  /* event 0xE: group forced to 12 (inferred) */
  event_or_team = get_event_type();
  if (event_or_team == EVENT_TURBO_TOURNAMENT) {
    record_value = 0xc;
  }
  /* loop over the 4 player slots: team_ptr walks g_player_slots[i].team (16-byte stride);
   * it ends when it passes the end of the array (address compare against g_ai_editor_view_origin_x, inferred) */
  team_ptr = &g_player_slots[0].team;
  do {
    event_or_team = *team_ptr;
    call_result = get_event_type();
    /* event 0x12: the expected team is the slot index itself */
    if (call_result == 0x12) {
      event_or_team = slot;
    }
    call_result = vehicle_get_active_flag(slot);
    if (call_result == 0) {
LAB_0040e842:
      mismatch_found = 1;
    }
    else {
      call_result = vehicle_get_object_field_80(slot);
      if (call_result != event_or_team + record_value * RECORD_GROUP_STRIDE) goto LAB_0040e842;
    }
    team_ptr = team_ptr + 4;
    slot = slot + 1;
    if (((char *)&g_ai_editor_view_origin_x + 0x3) < (char *)team_ptr) {
      return mismatch_found;
    }
  } while( true );
}
