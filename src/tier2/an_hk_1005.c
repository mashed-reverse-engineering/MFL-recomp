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
  int iVar6;
  int *piVar7;
  bool bVar8;
  struct {
    float local_20;
    float local_1c;
    float local_18;
    float local_14;
  } frame;
  
  player_count = get_player_count();
  iVar6 = 0;
  piVar7 = &g_player_stats_race[0].turbo_skill_kind;
  do {
    points = ((PlayerStatRecord *)(piVar7 + -3))->place;
    piVar7[-1] = 0;
    *piVar7 = 0;
    piVar7[1] = 0;
    piVar7[0x28] = 0;
    piVar7[0x3e] = 0;
    piVar7[0x3f] = 0;
    ((PlayerStatRecord *)(piVar7 + -3))->place = points + 1;
    piVar7[0x40] = 0;
    piVar7[0x41] = 0;
    piVar7[0x42] = 0;
    piVar7[0x43] = 0;
    piVar7[0x49] = 0;
    piVar7[0x4a] = 0;
    points = player_get_points(iVar6);
    if (points == 0) {
      piVar7[0x3b] = piVar7[0x3b] + 1;
    }
    if (player_count == 4) {
      if (points != 10) {
        bVar8 = points == 0xb;
LAB_00428785:
        if (!bVar8) goto LAB_0042878d;
      }
      piVar7[0x3a] = piVar7[0x3a] + 1;
    }
    else if ((player_count == 3) || (player_count == 2)) {
      bVar8 = points == 7;
      goto LAB_00428785;
    }
LAB_0042878d:
    team_mode = is_team_mode();
    if (((team_mode != 0) && (player_count == 4)) && (points == 7)) {
      piVar7[0x3a] = piVar7[0x3a] + 1;
    }
    piVar7 = piVar7 + 0x4e;
    iVar6 = iVar6 + 1;
    if ((char *)&g_player_stats_race + 0x4eb < (char *)piVar7) {
      frame.local_14 = (float)g_player_stats_race[0].aggression;
      frame.local_18 = (float)g_player_stats_race[1].aggression;
      fVar1 = frame.local_14;
      if (frame.local_14 < frame.local_18) {
        fVar1 = frame.local_18;
      }
      frame.local_1c = (float)g_player_stats_race[2].aggression;
      if (fVar1 < frame.local_1c) {
        fVar1 = frame.local_1c;
      }
      fVar2 = (float)g_player_stats_race[3].aggression;
      if (fVar1 < fVar2) {
        fVar1 = fVar2;
      }
      if (fVar1 == k_f_0) {
        frame.local_1c = 0.0;
        frame.local_18 = 0.0;
        frame.local_14 = 0.0;
        fVar1 = k_f_0;
      }
      else {
        fVar1 = float_1_0 / fVar1;
        frame.local_14 = fVar1 * frame.local_14;
        frame.local_18 = frame.local_18 * fVar1;
        frame.local_1c = frame.local_1c * fVar1;
        fVar1 = fVar1 * fVar2;
      }
      fVar2 = (float)g_player_stats_race[0].wall_hits;
      fVar3 = (float)g_player_stats_race[1].wall_hits;
      frame.local_20 = fVar2;
      if ((fVar3 < fVar2) && (g_player_stats_race[1].wall_hits != 0)) {
        frame.local_20 = fVar3;
      }
      fVar4 = (float)g_player_stats_race[2].wall_hits;
      if ((fVar4 < frame.local_20) && (g_player_stats_race[2].wall_hits != 0)) {
        frame.local_20 = fVar4;
      }
      fVar5 = (float)g_player_stats_race[3].wall_hits;
      if ((fVar5 < frame.local_20) && (g_player_stats_race[3].wall_hits != 0)) {
        frame.local_20 = fVar5;
      }
      if (frame.local_20 == k_f_0) {
        frame.local_1c = 0.0;
        frame.local_18 = 0.0;
        frame.local_14 = 0.0;
        fVar1 = k_f_0;
      }
      else {
        if (g_player_stats_race[0].wall_hits != 0) {
          frame.local_14 = float_1_0 / (fVar2 / frame.local_20);
        }
        if (g_player_stats_race[1].wall_hits != 0) {
          frame.local_18 = float_1_0 / (fVar3 / frame.local_20);
        }
        if (g_player_stats_race[2].wall_hits != 0) {
          frame.local_1c = float_1_0 / (fVar4 / frame.local_20);
        }
        if (g_player_stats_race[3].wall_hits != 0) {
          fVar1 = float_1_0 / (fVar5 / frame.local_20);
        }
      }
      debug_printf("cleanest line      %.2f   %.2f   %.2f   %.2f\n",(double)frame.local_14,
                   (double)frame.local_18,(double)frame.local_1c,(double)fVar1);
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


