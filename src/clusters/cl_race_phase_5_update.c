// cluster-entries: race_phase_5_update
/* Installed cluster race_phase_5_update (tools/cluster_install.py from output/cluster_pilot_hb1/race_phase_5_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fbe0*/ extern int race_get_setup_index();
/*0040fdd0*/ extern int race_set_phase();
/*00410060*/ extern int race_finish();
/*00414050*/ extern int time_trial_record_step();
/*004142e0*/ extern int ghost_update();
/*0041b810*/ extern int race_mark_ai_slots();
/*00420b10*/ extern int endpoint_panel_get_value();
/*0042b1a0*/ extern undefined __cdecl course_hook11(undefined4 p0);
/*0042e960*/ extern int intro_banner_set_active();
/*0042e980*/ extern int course_get_course_8c879c();
/*00434bb0*/ extern int get_event_type();
/*00435fb0*/ extern int ui_get_menu_69d2d0();
/*0047a100*/ extern int car_reset_state();
/*0047a7b0*/ extern int car_get_field_bf8();
/*0047a7d0*/ extern int car_get_field_bf4();
/*0047a830*/ extern int car_is_active();
/*0047b800*/ extern int car_turbo_spend();
/*0047b870*/ extern undefined __cdecl car_turbo_charge_update(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ddf54*/ extern float k_f_1_86;
/*006037c0*/ extern undefined4 g_phase_timer;
/*007eeb88*/ extern undefined4 g_race_7eeb88[0x1000];
/*007ef7dc*/ extern int g_race_clock_a;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*007ef800*/ extern int g_time_limit_ticks;
/*008d8b14*/ extern undefined4 g_race_setup;
/* race_phase_5_update */

/* "phase 5 handler of game_tick" */

/* Phase 5 (countdown, inferred) tick: finishes time-trial events when needed, counts g_phase_timer down by elapsed_ticks, resets/charges turbo per active car, and once the endpoint value reaches k_f_1_86 spends turbo, saves fields +bf4/+bf8 and switches to RACE_PHASE_RACING. */
void race_phase_5_update_n(int elapsed_ticks)

{
  int event_type;
  int car_active;
  undefined4 car_field;
  undefined4 *saved_turbo;
  int setup_off;
  float endpoint_value;
  
  endpoint_value = 0.0;
  event_type = course_get_course_8c879c();
  if (((event_type != 0) && (event_type = ui_get_menu_69d2d0(), event_type == 0)) &&
     ((event_type = get_event_type(), event_type == EVENT_TIME_TRIAL_0 ||
      ((event_type = get_event_type(), event_type == EVENT_TIME_TRIAL_1 ||
       (event_type = get_event_type(), event_type == EVENT_TIME_TRIAL_2)))))) {
    race_finish();
  }
  g_phase_timer = g_phase_timer - elapsed_ticks;
  g_race_7ef7fc = (uint)(g_time_limit_ticks < g_race_clock_a);
  event_type = race_get_setup_index();
  course_hook11(**(undefined4 **)(g_race_setup + 4 + event_type * 4));
  race_mark_ai_slots();
  event_type = 0;
  setup_off = 0xb8;
  do {
    if ((*(int *)(setup_off + g_race_setup) != 0) &&
       (car_active = car_is_active(event_type), car_active == 1)) {
      car_active = event_type;
      car_reset_state(car_active);
      car_turbo_charge_update(event_type,elapsed_ticks,car_active);
    }
    setup_off = setup_off + 4;
    event_type = event_type + 1;
  } while (setup_off < 200);
  endpoint_panel_get_value(&endpoint_value);
  if (k_f_1_86 <= endpoint_value) {
    event_type = 0;
    saved_turbo = g_race_7eeb88;
    setup_off = 0xb8;
    do {
      if ((*(int *)(setup_off + g_race_setup) != 0) &&
         (car_active = car_is_active(event_type), car_active == 1)) {
        car_turbo_spend(event_type);
        car_field = car_get_field_bf4(event_type);
        saved_turbo[-1] = car_field;
        car_field = car_get_field_bf8(event_type);
        *saved_turbo = car_field;
      }
      setup_off = setup_off + 4;
      event_type = event_type + 1;
      saved_turbo = saved_turbo + 3;
    } while (setup_off < 200);
    g_phase_timer = 0;
    event_type = get_event_type();
    if (((event_type == EVENT_TIME_TRIAL_0) || (event_type = get_event_type(), event_type == EVENT_TIME_TRIAL_1)) ||
       (event_type = get_event_type(), event_type == EVENT_TIME_TRIAL_2)) {
      intro_banner_set_active(1);
      ghost_update();
    }
    race_set_phase(RACE_PHASE_RACING);
  }
  event_type = get_event_type();
  if (((event_type != EVENT_TIME_TRIAL_0) && (event_type = get_event_type(), event_type != EVENT_TIME_TRIAL_1)) &&
     (event_type = get_event_type(), event_type != EVENT_TIME_TRIAL_2)) {
    return;
  }
  time_trial_record_step(0,0);
  return;
}

/* entry thunk at the original address of race_phase_5_update: register inputs (eax) -> natural C race_phase_5_update_n; ret 0 */
__declspec(naked) void race_phase_5_update(void)
{
    __asm {
        push eax
        call race_phase_5_update_n
        add esp, 4
        ret
    }
}
