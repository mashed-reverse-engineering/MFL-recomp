#include "mfl_tier2.h"
/*0040cc60*/ extern int player_get_points();
/*0040fdb0*/ extern int get_player_count();
/*00434a10*/ extern int is_team_mode();
/*004a8420*/ extern int debug_printf();
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/*008d8b30*/ extern undefined4 player_count;
#define PLAYER_STAT_STRIDE_WORDS (0x4e) /* sizeof(PlayerStatRecord)/4 */
/* player_stats_debug_log */

/* "no callers found: clears the per race counters of every player and prints 'cleanest line' /
   'number of pants' / 'number of crowns' lines (old debug code)" */

void player_stats_debug_log(void)

{
  float score3;
  float stat0;
  float stat1;
  float stat2;
  float stat3;
  int player_count;
  int points;
  int team_mode;
  int player;
  int *stat_word_ptr;
  bool is_match;
  /* score0..2 (+ score3 for player 3) are reused: first aggression, then wall hit ratios, printed as the "cleanest line" values */
  struct {
    float min_wall_hits;
    float score2;
    float score1;
    float score0;
  } frame;
  
  /* Dead debug code (no callers): for each of the 4 player stat records bump the place, clear per-race counters and count bonus flags from player_get_points; then print relative "cleanest line", pants, crowns and turbo lines. stat0..3 are reused: aggression of player 3 / wall hits of players 0..3 as float. */
  player_count = get_player_count();
  player = 0;
  stat_word_ptr = &g_player_stats_race[0].turbo_skill_kind;
  do {
    points = ((PlayerStatRecord *)(stat_word_ptr + -3))->place;
    stat_word_ptr[-1] = 0;
    *stat_word_ptr = 0;
    stat_word_ptr[1] = 0;
    stat_word_ptr[0x28] = 0;
    stat_word_ptr[0x3e] = 0;
    stat_word_ptr[0x3f] = 0;
    ((PlayerStatRecord *)(stat_word_ptr + -3))->place = points + 1;
    stat_word_ptr[0x40] = 0;
    stat_word_ptr[0x41] = 0;
    stat_word_ptr[0x42] = 0;
    stat_word_ptr[0x43] = 0;
    stat_word_ptr[0x49] = 0;
    stat_word_ptr[0x4a] = 0;
    points = player_get_points(player);
    if (points == 0) {
      stat_word_ptr[0x3b] = stat_word_ptr[0x3b] + 1;
    }
    if (player_count == 4) {
      if (points != 10) {
        is_match = points == 0xb;
LAB_00428785:
        if (!is_match) goto LAB_0042878d;
      }
      stat_word_ptr[0x3a] = stat_word_ptr[0x3a] + 1;
    }
    else if ((player_count == 3) || (player_count == 2)) {
      is_match = points == 7;
      goto LAB_00428785;
    }
LAB_0042878d:
    team_mode = is_team_mode();
    if (((team_mode != 0) && (player_count == 4)) && (points == 7)) {
      stat_word_ptr[0x3a] = stat_word_ptr[0x3a] + 1;
    }
    stat_word_ptr = stat_word_ptr + PLAYER_STAT_STRIDE_WORDS;
    player = player + 1;
    if ((char *)&g_player_stats_race + 0x4eb < (char *)stat_word_ptr) {
      frame.score0 = (float)g_player_stats_race[0].aggression;
      frame.score1 = (float)g_player_stats_race[1].aggression;
      score3 = frame.score0;
      if (frame.score0 < frame.score1) {
        score3 = frame.score1;
      }
      frame.score2 = (float)g_player_stats_race[2].aggression;
      if (score3 < frame.score2) {
        score3 = frame.score2;
      }
      stat0 = (float)g_player_stats_race[3].aggression;
      if (score3 < stat0) {
        score3 = stat0;
      }
      if (score3 == k_f_0) {
        frame.score2 = 0.0;
        frame.score1 = 0.0;
        frame.score0 = 0.0;
        score3 = k_f_0;
      }
      else {
        score3 = float_1_0 / score3;
        frame.score0 = score3 * frame.score0;
        frame.score1 = frame.score1 * score3;
        frame.score2 = frame.score2 * score3;
        score3 = score3 * stat0;
      }
      stat0 = (float)g_player_stats_race[0].wall_hits;
      stat1 = (float)g_player_stats_race[1].wall_hits;
      frame.min_wall_hits = stat0;
      if ((stat1 < stat0) && (g_player_stats_race[1].wall_hits != 0)) {
        frame.min_wall_hits = stat1;
      }
      stat2 = (float)g_player_stats_race[2].wall_hits;
      if ((stat2 < frame.min_wall_hits) && (g_player_stats_race[2].wall_hits != 0)) {
        frame.min_wall_hits = stat2;
      }
      stat3 = (float)g_player_stats_race[3].wall_hits;
      if ((stat3 < frame.min_wall_hits) && (g_player_stats_race[3].wall_hits != 0)) {
        frame.min_wall_hits = stat3;
      }
      if (frame.min_wall_hits == k_f_0) {
        frame.score2 = 0.0;
        frame.score1 = 0.0;
        frame.score0 = 0.0;
        score3 = k_f_0;
      }
      else {
        if (g_player_stats_race[0].wall_hits != 0) {
          frame.score0 = float_1_0 / (stat0 / frame.min_wall_hits);
        }
        if (g_player_stats_race[1].wall_hits != 0) {
          frame.score1 = float_1_0 / (stat1 / frame.min_wall_hits);
        }
        if (g_player_stats_race[2].wall_hits != 0) {
          frame.score2 = float_1_0 / (stat2 / frame.min_wall_hits);
        }
        if (g_player_stats_race[3].wall_hits != 0) {
          score3 = float_1_0 / (stat3 / frame.min_wall_hits);
        }
      }
      debug_printf("cleanest line      %.2f   %.2f   %.2f   %.2f\n",(double)frame.score0,
                   (double)frame.score1,(double)frame.score2,(double)score3);
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


