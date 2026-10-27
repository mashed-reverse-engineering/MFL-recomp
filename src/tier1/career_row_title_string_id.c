#include "mfl_tier2.h"
/*006091a4*/ extern undefined g_career_row_6091a4;
/*006091b8*/ extern undefined g_career_row_6091b8;
/*006091cc*/ extern undefined g_career_row_6091cc;
/*006091e0*/ extern undefined g_career_row_6091e0;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d688*/ extern undefined4 g_career_row;


/* career_row_title_string_id */

/* "(row or -1): string id of the event title for the current event type" */

int __cdecl career_row_title_string_id(int row)
{
  if (row < 0) {
    row = g_career_row;
  }
  if (g_event_type == EVENT_TURBO_TOURNAMENT) {
    return *(int *)(&g_career_row_6091a4 + row * 4);
  }
  if (g_event_type == EVENT_ICE_TOURNAMENT) {
    return *(int *)(&g_career_row_6091b8 + row * 4);
  }
  if (g_event_type == EVENT_NIGHT_TOURNAMENT) {
    return *(int *)(&g_career_row_6091cc + row * 4);
  }
  if (g_event_type == EVENT_TRAINING) {
    return *(int *)(&g_career_row_6091e0 + row * 4);
  }
  return row + 0x4a;
}
