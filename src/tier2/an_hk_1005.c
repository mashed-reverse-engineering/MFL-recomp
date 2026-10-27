#include "mfl_tier2.h"
/*0040cc60*/ extern int player_get_points();
/*0040fdb0*/ extern int get_player_count();
/*00434a10*/ extern int is_team_mode();
/*004a8420*/ extern int debug_printf();
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/*008d8b30*/ extern undefined4 player_count;
/* player_stats_debug_log */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "no callers found: clears the per race counters of every player and prints 'cleanest line' /
   'number of pants' / 'number of crowns' lines (old debug code)" */

void player_stats_debug_log(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int player_count;
  int points;
  int team_mode;
  int player;
  int *stat_ptr;
  bool is_match;
  /* score0..2 (+ fVar1 for player 3) are reused: first aggression, then wall hit ratios, printed as the "cleanest line" values */
  struct {
    float min_wall_hits;
    float score2;
    float score1;
    float score0;
  } frame;
  
  player_count = get_player_count();
  player = 0;
  stat_ptr = &g_player_stats_race[0].turbo_skill_kind;
  do {
    points = ((PlayerStatRecord *)(stat_ptr + -3))->place;
    stat_ptr[-1] = 0;
    *stat_ptr = 0;
    stat_ptr[1] = 0;
    stat_ptr[0x28] = 0;
    stat_ptr[0x3e] = 0;
    stat_ptr[0x3f] = 0;
    ((PlayerStatRecord *)(stat_ptr + -3))->place = points + 1;
    stat_ptr[0x40] = 0;
    stat_ptr[0x41] = 0;
    stat_ptr[0x42] = 0;
    stat_ptr[0x43] = 0;
    stat_ptr[0x49] = 0;
    stat_ptr[0x4a] = 0;
    points = player_get_points(player);
    if (points == 0) {
      stat_ptr[0x3b] = stat_ptr[0x3b] + 1;
    }
    if (player_count == 4) {
      if (points != 10) {
        is_match = points == 0xb;
LAB_00428785:
        if (!is_match) goto LAB_0042878d;
      }
      stat_ptr[0x3a] = stat_ptr[0x3a] + 1;
    }
    else if ((player_count == 3) || (player_count == 2)) {
      is_match = points == 7;
      goto LAB_00428785;
    }
LAB_0042878d:
    team_mode = is_team_mode();
    if (((team_mode != 0) && (player_count == 4)) && (points == 7)) {
      stat_ptr[0x3a] = stat_ptr[0x3a] + 1;
    }
    stat_ptr = stat_ptr + 0x4e;
    player = player + 1;
    if ((char *)&g_player_stats_race + 0x4eb < (char *)stat_ptr) {
      frame.score0 = (float)g_player_stats_race[0].aggression;
      frame.score1 = (float)g_player_stats_race[1].aggression;
      fVar1 = frame.score0;
      if (frame.score0 < frame.score1) {
        fVar1 = frame.score1;
      }
      frame.score2 = (float)g_player_stats_race[2].aggression;
      if (fVar1 < frame.score2) {
        fVar1 = frame.score2;
      }
      fVar2 = (float)g_player_stats_race[3].aggression;
      if (fVar1 < fVar2) {
        fVar1 = fVar2;
      }
      if (fVar1 == k_f_0) {
        frame.score2 = 0.0;
        frame.score1 = 0.0;
        frame.score0 = 0.0;
        fVar1 = k_f_0;
      }
      else {
        fVar1 = float_1_0 / fVar1;
        frame.score0 = fVar1 * frame.score0;
        frame.score1 = frame.score1 * fVar1;
        frame.score2 = frame.score2 * fVar1;
        fVar1 = fVar1 * fVar2;
      }
      fVar2 = (float)g_player_stats_race[0].wall_hits;
      fVar3 = (float)g_player_stats_race[1].wall_hits;
      frame.min_wall_hits = fVar2;
      if ((fVar3 < fVar2) && (g_player_stats_race[1].wall_hits != 0)) {
        frame.min_wall_hits = fVar3;
      }
      fVar4 = (float)g_player_stats_race[2].wall_hits;
      if ((fVar4 < frame.min_wall_hits) && (g_player_stats_race[2].wall_hits != 0)) {
        frame.min_wall_hits = fVar4;
      }
      fVar5 = (float)g_player_stats_race[3].wall_hits;
      if ((fVar5 < frame.min_wall_hits) && (g_player_stats_race[3].wall_hits != 0)) {
        frame.min_wall_hits = fVar5;
      }
      if (frame.min_wall_hits == k_f_0) {
        frame.score2 = 0.0;
        frame.score1 = 0.0;
        frame.score0 = 0.0;
        fVar1 = k_f_0;
      }
      else {
        if (g_player_stats_race[0].wall_hits != 0) {
          frame.score0 = float_1_0 / (fVar2 / frame.min_wall_hits);
        }
        if (g_player_stats_race[1].wall_hits != 0) {
          frame.score1 = float_1_0 / (fVar3 / frame.min_wall_hits);
        }
        if (g_player_stats_race[2].wall_hits != 0) {
          frame.score2 = float_1_0 / (fVar4 / frame.min_wall_hits);
        }
        if (g_player_stats_race[3].wall_hits != 0) {
          fVar1 = float_1_0 / (fVar5 / frame.min_wall_hits);
        }
      }
      debug_printf("cleanest line      %.2f   %.2f   %.2f   %.2f\n",(double)frame.score0,
                   (double)frame.score1,(double)frame.score2,(double)fVar1);
      debug_printf("number of pants    %d      %d      %d      %d\n",g_player_stats_race[0].pants,
                   g_player_stats_race[1].pants,g_player_stats_race[2].pants,
                   g_player_stats_race[3].pants);
      debug_printf("number of crowns   %d      %d      %d      %d\n",g_player_stats_race[0].crowns,
                   g_player_stats_race[1].crowns,g_player_stats_race[2].crowns,
                   g_player_stats_race[3].crowns);
      debug_printf("turbo effective    %d      %d      %d      %d\n",g_player_stats_race[0].turbo_used,
                   g_player_stats_race[1].turbo_used,g_player_stats_race[2].turbo_used,
                   g_player_stats_race[3].turbo_used);
      return;
    }
  } while( true );
}


