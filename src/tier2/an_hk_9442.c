#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type(int slot);
/*00434bb0*/ extern int get_event_type(void);
/*00435f10*/ extern int get_career_row(void);
/*0044c180*/ extern double __cdecl player_distance_get(int player);
/*0047b520*/ extern void car_current_matrix(char **matrix, int car);
/*005dd33c*/ extern float k_f_4;
/*005dd814*/ extern float k_f_6;
/*005de224*/ extern float k_f_5_5;
/*005de228*/ extern float k_f_6_5;
/*005e64a4*/ extern float k_f_0;
/*006039a8*/ extern int g_ai_drive_6039a8[];
/*007ef7f0*/ extern int g_tick_count;
/*008c9980*/ extern float g_ai_skill;
/*008c9984*/ extern int g_ai_last_active_player;
/*008c9988*/ extern int g_ai_catchup_class;
/*008c9994*/ extern int g_car_8c9994;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/* ai_catchup_bot_state_update */
/* "rubberband: accumulates time and picks the target distance depending on the human player distance" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
int __cdecl ai_catchup_bot_state_update(int unused1, int unused2, int *target, int bot)
{
  int skill;
  int i;
  int human;
  int value;
  double distance;
  char *matrix;

  if (g_ai_catchup_class == 2) {
    return 0;
  }
  skill = (int)g_ai_skill;
  if (g_ai_last_active_player != -1 && race_setup_get_slot_type(g_ai_last_active_player) == 1) {
    skill = skill + 3;
    if (skill > 10) {
      skill = 10;
    }
  }
  if (get_event_type() != EVENT_CUP_3) {
    return 0;
  }
  if (get_career_row() != 0) {
    return 0;
  }
  if (g_ai_bots[bot].catchup_cycles >= g_ai_drive_6039a8[g_car_8c9994 + skill * 5]) {
    return 0;
  }
  if (player_distance_get(bot) == k_f_0) {
    human = -1;
    for (i = 0; i < 4; i++) {
      if (race_setup_get_slot_type(i) == 1) {
        human = i;
      }
    }
    if (human == -1) {
      return 0;
    }
    distance = player_distance_get(human);
    if (distance > k_f_6_5) {
      g_ai_bots[bot].catchup_ticks = g_ai_bots[bot].catchup_ticks + g_tick_count;
      car_current_matrix(&matrix, bot);
      target[0] = *(int *)(matrix + 0x30);
      target[1] = *(int *)(matrix + 0x38);
      if (g_ai_bots[bot].catchup_ticks > 10000) {
        g_ai_bots[bot].catchup_ticks = 0;
        g_ai_bots[bot].catchup_cycles = g_ai_bots[bot].catchup_cycles + 1;
      }
      return 1;
    }
    if (distance > k_f_5_5 && g_ai_bots[bot].catchup_ticks != 0) {
      g_ai_bots[bot].catchup_ticks = g_ai_bots[bot].catchup_ticks + g_tick_count;
    }
    value = 1;
    if (!(distance < k_f_6)) {
      value = 0;
    }
    if (distance < k_f_4) {
      if (g_ai_bots[bot].catchup_ticks == 0) {
        return 0;
      }
    }
    else if (value == 1) {
      if (g_ai_bots[bot].catchup_ticks <= 0x1194) {
        return 0;
      }
    }
    else {
      return 0;
    }
  }
  else if (g_ai_bots[bot].catchup_ticks <= 0x1194) {
    return 0;
  }
  g_ai_bots[bot].catchup_ticks = 0;
  g_ai_bots[bot].catchup_cycles = g_ai_bots[bot].catchup_cycles + 1;
  return 0;
}
