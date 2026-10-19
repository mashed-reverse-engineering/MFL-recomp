#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*00468620*/ extern int voice_queue_reset_all();
/*00468760*/ extern int sound_phase_change_check();
/*00468bf0*/ extern int voice_queue_clear_players_when_phase_2();
/*00468c20*/ extern int sound_event_log_find_current_tick();
/*00468fa0*/ extern int voice_queue_advance_timers();
/*00469210*/ extern int voice_queue_final_lines();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*006aaf64*/ extern undefined4 g_car_6aaf64[0x1000];
/*006aaf74*/ extern char g_weapon_6aaf74[];

/* race phases that take the reseed branch below (inferred: not in names/MFL_enums.txt, named by the branch they select) */
#define RACE_PHASE_RESEED_1 (7)
#define RACE_PHASE_RESEED_2 (0xb)
/* upper bound of rp_random_int_range(0, N), inclusive: values 0..250 (inferred) */
#define VOICE_QUEUE_RANDOM_UPPER (0xfa)

/* voice_queue_race_update */

/* "race phase dependent queue update (final lines, removals, resets)" */

/* Per-update voice queue housekeeping, selected by the current race phase (no parameters, no return value).
 * Always: voice_queue_final_lines() and voice_queue_clear_players_when_phase_2().
 * Normal phases: log tick, advance timers, phase change check, reset all.
 * Reseed phases (RACE_PHASE_RESEED_1 / _2): fill the first 4 entries of g_car_6aaf64 with random values
 * 0..VOICE_QUEUE_RANDOM_UPPER, then phase change check and reset all. */

void voice_queue_race_update(void)

{
  int race_phase;
  undefined4 random_value;
  undefined4 *entry;

  race_phase = race_get_phase();
  voice_queue_final_lines();
  voice_queue_clear_players_when_phase_2();
  /* normal phases: advance the voice queue timers and reset it */
  if ((race_phase != RACE_PHASE_RESEED_1) && (race_phase != RACE_PHASE_RESEED_2)) {
    sound_event_log_find_current_tick();
    voice_queue_advance_timers();
    sound_phase_change_check();
    voice_queue_reset_all();
    return;
  }
  /* reseed: walk g_car_6aaf64 up to the address of g_weapon_6aaf74 (inferred: 0x10 bytes, so only 4 entries
     are written, not the whole 0x1000-element array) */
  entry = g_car_6aaf64;
  do {
    random_value = rp_random_int_range(0,VOICE_QUEUE_RANDOM_UPPER);
    *entry = random_value;
    entry = entry + 1;
  } while ((int)entry < ((int)&g_weapon_6aaf74));
  sound_phase_change_check();
  voice_queue_reset_all();
  return;
}


