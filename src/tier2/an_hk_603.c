#include "mfl_tier2.h"
/*00403b30*/ extern undefined __cdecl pause_input_update(undefined4 p0);
/*004042d0*/ extern int bomb_update();
/*0040fdc0*/ extern int race_get_phase();
/*004165a0*/ extern int trail_reset();
/*0041d2d0*/ extern int hudpanel_bomb_meter_update();
/*0041e5c0*/ extern int score_panel_update();
/*0041f110*/ extern int team_panel_update();
/*0041f200*/ extern int hudpanel_fuel_meter_update();
/*0041fcd0*/ extern int kc_panel_update();
/*004208b0*/ extern int endpoint_panel_update();
/*00420d50*/ extern int hudpanel_score_digits_update();
/*004217b0*/ extern int hudpanel_load_indicator_update();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00437f10*/ extern int training_lesson_get();
/*0050b5e0*/ extern int nop_stub();
/*00652f68*/ extern undefined4 g_race_phase;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7ec*/ extern float g_frame_seconds;

/* game_mode values that pick a HUD meter (inferred from the panel each case calls) */
#define GAME_MODE_FUEL_METER (5)
#define GAME_MODE_BOMB_METER (10)
/* race phase in which the bomb update runs (inferred from bomb_update) */
#define RACE_PHASE_BOMB (6)

/* race_update_by_event_type */

/* "for phase 7/10/11 resets the trail, then switches on the event type to the per event update" */

/*
 * Per-frame race HUD update. Resets the trail in race phases 7, 10 and 11, then
 * picks the HUD panels to update from the current event type and game mode.
 * Takes no parameters and returns nothing.
 */
void race_update_by_event_type(void)

{
  undefined4 event_type;
  int result;
  bool solo_panels;

  /* phases 7, 10 and 11 reset the trail (inferred from the trail_reset call) */
  if ((g_race_phase == 7) || ((9 < (int)g_race_phase && ((int)g_race_phase < 0xc)))) {
    nop_stub(0);
    trail_reset();
  }
  else {
    nop_stub(1);
  }
  event_type = get_event_type();
  /* event type selects the update: 0-2 load indicator only; 3-5 and 14-16 meters or
     score digits by game mode; 6-8 and 11-13 team or score panels; 17 score digits
     for training lesson 4; 18 pauses input, then falls through to the panels */
  switch(event_type) {
  case 0:
  case 1:
  case 2:
    hudpanel_load_indicator_update();
    return;
  case 3:
  case 4:
  case 5:
  case 0xe:
  case 0xf:
  case 0x10:
    switch(game_mode) {
    case 4:
    case 7:
    case 8:
    case 9:
      goto switchD_00411d2b_caseD_4;
    case GAME_MODE_FUEL_METER:
      hudpanel_fuel_meter_update();
      return;
    case GAME_MODE_BOMB_METER:
      hudpanel_bomb_meter_update();
      result = race_get_phase();
      if (result == RACE_PHASE_BOMB) {
        bomb_update();
        return;
      }
      return;
    }
    break;
  case 6:
  case 7:
  case 8:
  case 0xb:
  case 0xc:
  case 0xd:
    result = is_team_mode();
    if (result == 0) goto LAB_00411d7c;
    solo_panels = game_mode == 2;
    goto LAB_00411d70;
  case 0x11:
    result = training_lesson_get();
    if (result != 4) goto LAB_00411d7c;
switchD_00411d2b_caseD_4:
    hudpanel_score_digits_update();
    return;
  case 0x12:
    pause_input_update(*(int *)&g_frame_seconds);
  }
  result = is_team_mode();
  solo_panels = result == 0;
LAB_00411d70:
  /* team mode shows the team and endpoint panels; otherwise the score and kc panels */
  if (!solo_panels) {
    team_panel_update();
    endpoint_panel_update();
    return;
  }
LAB_00411d7c:
  score_panel_update();
  kc_panel_update();
  return;
}


