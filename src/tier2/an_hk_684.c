#include "mfl_tier2.h"
/*004685b0*/ extern int voice_queue_select_tables();
/*005e64a4*/ extern float k_f_0;
/*008be200*/ extern undefined g_weapon_8be200;
/*008be7e0*/ extern undefined g_sound_8be7e0;
/*008bed04*/ extern char *g_voice_queue_count_ptr;
/*008bed08*/ extern char *g_voice_queue_entries_ptr;

/* byte offset of the float "value" field inside a voice queue entry (inferred) */
#define VOICE_ENTRY_VALUE_OFFSET (0x14)
/* size of one voice queue entry, in floats (40 bytes) (inferred) */
#define VOICE_ENTRY_STRIDE_FLOATS (10)
/* number of teams with a voice queue slot */
#define TEAM_COUNT (4)

/* voice_queue_reset_all
 *
 * For each team (0..TEAM_COUNT-1): selects that team's voice queue tables via
 * voice_queue_select_tables(team), clears the team's flag in g_weapon_8be200,
 * then scans the team's voice queue entries for the one whose value field is
 * the largest and above k_f_0 (inferred). If found, the flag is set to 1 and
 * g_sound_8be7e0[team] points at that entry's record start (5 floats before its
 * value field, inferred). Takes no parameters, returns nothing.
 * (was: "voice_queue_select_tables for each team and clears its count")
 */

void voice_queue_reset_all(void)

{
  int team;
  float best_value;
  int *entry_count_ptr;
  float *entry;
  int i;

  i = 0;
  /* per team: select its tables and reset its flag */
  do {
    team = (int)(i);
    voice_queue_select_tables(i);
    entry_count_ptr = (int *)g_voice_queue_count_ptr;
    best_value = k_f_0;
    ((undefined4 *)&g_weapon_8be200)[team] = 0;
    i = 0;
    /* scan this team's entries, keeping the largest value above k_f_0 */
    if (0 < *entry_count_ptr) {
      entry = (float *)(g_voice_queue_entries_ptr + VOICE_ENTRY_VALUE_OFFSET);
      do {
        if (best_value < *entry) {
          *(float **)((char *)&g_sound_8be7e0 + team * 4) = entry + -5;
          ((undefined4 *)&g_weapon_8be200)[team] = 1;
          best_value = *entry;
        }
        i = i + 1;
        entry = entry + VOICE_ENTRY_STRIDE_FLOATS;
      } while (i < *entry_count_ptr);
    }
    i = team + 1;
  } while (i < TEAM_COUNT);
  return;
}


