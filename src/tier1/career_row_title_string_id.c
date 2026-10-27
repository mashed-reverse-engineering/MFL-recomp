#include "mfl_tier2.h"
/*006091a4*/ extern undefined g_career_row_6091a4;
/*006091b8*/ extern undefined g_career_row_6091b8;
/*006091cc*/ extern undefined g_career_row_6091cc;
/*006091e0*/ extern undefined g_career_row_6091e0;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d688*/ extern undefined4 g_career_row;


/* career_row_title_string_id */

/* "(row or -1): string id of the event title for the current event type" */

int __cdecl career_row_title_string_id(int param_1)
{
  if (param_1 < 0) {
    param_1 = g_career_row;
  }
  if (g_event_type == EVENT_TURBO_TOURNAMENT) {
    return *(int *)(&g_career_row_6091a4 + param_1 * 4);
  }
  if (g_event_type == EVENT_ICE_TOURNAMENT) {
    return *(int *)(&g_career_row_6091b8 + param_1 * 4);
  }
  if (g_event_type == EVENT_NIGHT_TOURNAMENT) {
    return *(int *)(&g_career_row_6091cc + param_1 * 4);
  }
  if (g_event_type == EVENT_TRAINING) {
    return *(int *)(&g_career_row_6091e0 + param_1 * 4);
  }
  return param_1 + 0x4a;
}
