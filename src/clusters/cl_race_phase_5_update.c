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

/* WARNING: Unknown calling convention */
/* "phase 5 handler of game_tick" */

void race_phase_5_update_n(int in_eax)

{
  int event_type;
  int car_active;
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  float local_4;
  
  local_4 = 0.0;
  event_type = course_get_course_8c879c();
  if (((event_type != 0) && (event_type = ui_get_menu_69d2d0(), event_type == 0)) &&
     ((event_type = get_event_type(), event_type == 0 ||
      ((event_type = get_event_type(), event_type == 1 ||
       (event_type = get_event_type(), event_type == 2)))))) {
    race_finish();
  }
  g_phase_timer = g_phase_timer - in_eax;
  g_race_7ef7fc = (uint)(g_time_limit_ticks < g_race_clock_a);
  event_type = race_get_setup_index();
  course_hook11(**(undefined4 **)(g_race_setup + 4 + event_type * 4));
  race_mark_ai_slots();
  event_type = 0;
  iVar3 = 0xb8;
  do {
    if ((*(int *)(iVar3 + g_race_setup) != 0) &&
       (car_active = car_is_active(event_type), car_active == 1)) {
      car_active = event_type;
      car_reset_state(car_active);
      car_turbo_charge_update(event_type,in_eax,car_active);
    }
    iVar3 = iVar3 + 4;
    event_type = event_type + 1;
  } while (iVar3 < 200);
  endpoint_panel_get_value(&local_4);
  if (k_f_1_86 <= local_4) {
    event_type = 0;
    puVar2 = g_race_7eeb88;
    iVar3 = 0xb8;
    do {
      if ((*(int *)(iVar3 + g_race_setup) != 0) &&
         (car_active = car_is_active(event_type), car_active == 1)) {
        car_turbo_spend(event_type);
        uVar1 = car_get_field_bf4(event_type);
        puVar2[-1] = uVar1;
        uVar1 = car_get_field_bf8(event_type);
        *puVar2 = uVar1;
      }
      iVar3 = iVar3 + 4;
      event_type = event_type + 1;
      puVar2 = puVar2 + 3;
    } while (iVar3 < 200);
    g_phase_timer = 0;
    event_type = get_event_type();
    if (((event_type == 0) || (event_type = get_event_type(), event_type == 1)) ||
       (event_type = get_event_type(), event_type == 2)) {
      intro_banner_set_active(1);
      ghost_update();
    }
    race_set_phase(6);
  }
  event_type = get_event_type();
  if (((event_type != 0) && (event_type = get_event_type(), event_type != 1)) &&
     (event_type = get_event_type(), event_type != 2)) {
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
