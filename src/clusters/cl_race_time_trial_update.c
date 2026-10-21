// cluster-entries: race_time_trial_update
/* Installed cluster race_time_trial_update (tools/cluster_install.py from output/cluster_pilot_uc3/race_time_trial_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040f6f0*/ extern int race_get_intro_652df0(void);
/*0040f730*/ extern void intro_banner_update(void);
/*00430ac0*/ extern short screen_height(void);
/*00434bb0*/ extern int get_event_type(void);
/*00409f30*/ extern float player_progress_b(int player);
/*0040fdc0*/ extern int race_get_phase(void);
/*0040fdd0*/ extern void race_set_phase(int phase);
/*004a8420*/ extern int debug_printf(const char *format, ...);
/*00413b10*/ extern void replay_current_get_split_time(int which, float *a, int *b, int *c);
/*00413ad0*/ extern void replay_best_get_split_time(int which, float *a, int *b, int *c);
/*004364c0*/ extern void ui_set_triple(int row, int column, float a, int b, int c);
/*0042b110*/ extern int current_course_id(void);
/*0042d9f0*/ extern void time_trial_default_record_a(int course, float *a, int *b, int *c);
/*0042df90*/ extern void time_trial_default_record_b(int course, float *a, int *b, int *c);
/*00413dd0*/ extern void replay_load_compare(int which);
/*004138a0*/ extern int time_split(int clock, float *seconds, int *minutes, int *index);
extern float g_intro_banner_slide_step;
extern int g_intro_banner_offset;
extern int g_course_8c879c;
extern float g_course_69bb6c;
extern int g_course_69bb64;
extern int g_course_69bb5c;
extern int g_race_clock_a;
extern char g_race_time_5de8c8[];

/* Time trial per frame update: the intro banner (offset, slide step), then for the events 0 / 1 / 2 the progress stage g_course_8c879c (0 = off, 1..10 = the
   ten percent intervals of the player's progress, 11 = finished): when the race is in phase 6 at stage 11 the final times are logged, the split times of the
   current and best replay are stored (ui_set_triple rows 0 / 1, default records for the course) and the phase becomes 7; below stage 11 the running
   time is split into (seconds, minutes, index) and capped at 9:59.99. */
void race_time_trial_update(void)
{
  struct { int c; int b; float a; } triple;      /* the three values the replay functions return, also scratch for the int to float conversions */
  int stage;

  if (race_get_intro_652df0() != 0) {
    intro_banner_update();
  }
  g_intro_banner_offset = g_intro_banner_offset + 1;
  if (g_intro_banner_slide_step != 0.0f) {
    triple.b = screen_height();
    g_intro_banner_slide_step = (float)triple.b * 0.025f + g_intro_banner_slide_step;
    if (g_intro_banner_slide_step >= 0.0f) {
      g_intro_banner_slide_step = 0.0f;
    }
  }
  if (get_event_type() != 0 && get_event_type() != 1 && get_event_type() != 2) {
    return;
  }
  triple.a = player_progress_b(0);
  stage = g_course_8c879c;
  if (stage == 0) {
    return;
  }
  triple.b = stage * 10;
  if ((float)triple.b < triple.a && (triple.b = triple.b + 10, (float)triple.b > triple.a)) {
    g_course_8c879c = stage + 1;
  } else if (stage == 10) {
    if (triple.a < 10.0f) {
      g_course_8c879c = stage + 1;
    } else {
      goto split;
    }
  }
  stage = g_course_8c879c;
  if (stage == 0xb) {
    if (race_get_phase() == 6) {
      debug_printf("time trial time is %f\n", (double)((float)g_course_69bb64 + g_course_69bb6c));
      debug_printf(g_race_time_5de8c8, (double)((float)g_race_clock_a * 0.00033333333f));
      replay_current_get_split_time(0, &triple.a, &triple.b, &triple.c);
      ui_set_triple(0, 0, triple.a, triple.b, triple.c);
      replay_current_get_split_time(1, &triple.a, &triple.b, &triple.c);
      ui_set_triple(1, 0, triple.a, triple.b, triple.c);
      time_trial_default_record_a(current_course_id(), &triple.a, &triple.b, &triple.c);
      replay_best_get_split_time(0, &triple.a, &triple.b, &triple.c);
      ui_set_triple(0, 1, triple.a, triple.b, triple.c);
      time_trial_default_record_b(current_course_id(), &triple.a, &triple.b, &triple.c);
      replay_best_get_split_time(1, &triple.a, &triple.b, &triple.c);
      ui_set_triple(1, 1, triple.a, triple.b, triple.c);
      replay_load_compare(1);
      race_set_phase(7);
    }
    if (g_course_8c879c >= 0xb) {
      return;
    }
  } else if (stage > 0xb) {
    return;
  }
split:
  time_split(g_race_clock_a, &g_course_69bb6c, &g_course_69bb64, &g_course_69bb5c);
  if (g_course_69bb5c >= 10) {
    g_course_69bb6c = 0.99f;
    g_course_69bb64 = 0x3b;
    g_course_69bb5c = 9;
  }
}

