#include "mfl_tier2.h"
/*0040fdd0*/ extern int race_set_phase();
/*0041e5a0*/ extern int score_panel_reset_entries();
/*00420990*/ extern int endpoint_panel_clear();
/*00421150*/ extern int hud_game_vars_clear();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*0050b5e0*/ extern int nop_stub();
/*007ef7b8*/ extern undefined4 game_mode;

/* race phase id that phase 4 hands over to (inferred from race_set_phase calls) */
#define RACE_PHASE_NEXT (0x5)

/* race_phase_4_update */

/* phase 4: waits for the countdown then changes phase */

/*
 * Update for race phase 4: waits for the countdown, then moves the race to RACE_PHASE_NEXT.
 * param_1: not used by the body (inferred); a char * to the caller's race context.
 * Returns nothing.
 */
void __cdecl race_phase_4_update(char *param_1)

{
  undefined4 event_type;
  int team_mode;
  bool reset_score_panel;

  endpoint_panel_clear();
  event_type = get_event_type();
  /* event types 0-2: no countdown to wait for, clear the HUD and change phase at once */
  switch(event_type) {
  case 0:
  case 1:
  case 2:
    hud_game_vars_clear();
    race_set_phase(RACE_PHASE_NEXT);
    return;
  default:
    /* other event types: reset the score panel only when not in team mode */
    team_mode = is_team_mode();
    reset_score_panel = team_mode == 0;
    break;
  /* event types 6-8, 0xb-0xd: in team mode, reset the score panel only when game_mode == GAME_MODE_HARE_AND_HOUNDS */
  case 6:
  case 7:
  case 8:
  case 0xb:
  case 0xc:
  case 0xd:
    team_mode = is_team_mode();
    if (team_mode == 0) goto LAB_004124cc;
    reset_score_panel = game_mode == GAME_MODE_HARE_AND_HOUNDS;
  }
  if (!reset_score_panel) {
    nop_stub();
    race_set_phase(RACE_PHASE_NEXT);
    return;
  }
LAB_004124cc:
  score_panel_reset_entries();
  race_set_phase(RACE_PHASE_NEXT);
  return;
}


