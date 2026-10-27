// cluster-entries: race_check_winner_by_game_mode
/* Installed cluster race_check_winner_by_game_mode (tools/cluster_install.py from output/cluster_pilot_kb/race_check_winner_by_game_mode.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef int CourseId;
/*00406cf0*/ extern int fx_objects_all_exploded();
/*0040cc60*/ extern int player_get_points();
/*0040fdb0*/ extern int get_player_count();
/*0040fdd0*/ extern int race_set_phase();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*0042b110*/ extern enum CourseId current_course_id();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00451c50*/ extern int camera_records_warmup();
/*00459090*/ extern undefined __cdecl course_set_lod(undefined4 p0);
/*00464db0*/ extern int powerups_set_all_enabled();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*005dd32c*/ extern float k_f_3;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005ddf58*/ extern float k_f_1_999;
/*00652df0*/ extern undefined4 g_intro_652df0;
/*00652df4*/ extern float g_intro_652df4;
/*00652df8*/ extern undefined4 g_car_652df8;
/*00652dfc*/ extern float g_intro_652dfc;
/*00652e00*/ extern float g_game_652e00;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7cc*/ extern float g_course_7ef7cc;
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b30*/ extern undefined4 player_count;
/*0041a370*/ extern float ai_get_counter();
/* race_check_winner_by_game_mode */
#define COURSE_HIGHWAYN (12)
#define COURSE_HIGHWAY (38)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes the winning player (points 8/12 thresholds, team mode, per game mode 4,5,7,8,9) into
   g_intro_652df0" */

int race_check_winner_by_game_mode(void)

{
  bool bVar1;
  int iVar2;
  int points;
  int iVar3;
  int team_mode;
  CourseId course_id;
  int car_active;
  float10 counter;
  struct {
    int local_c;
    undefined1 local_8 [4];
    float fStack_4;
  } frame;
  
  car_active = 0;
  g_intro_652df0 = 0;
  iVar2 = get_event_type();
  if ((((iVar2 == 0) || (iVar2 = get_event_type(), iVar2 == 1)) ||
      (iVar2 = get_event_type(), iVar2 == 2)) || (iVar2 = get_event_type(), iVar2 == 0x11)) {
    return 0;
  }
  iVar2 = 1;
  do {
    points = player_get_points(iVar2 + -1);
    if ((((player_count == 2) || (player_count == 3)) || (iVar3 = is_team_mode(), iVar3 != 0)) &&
       (points == 8)) {
      car_active = iVar2;
    }
    if (((game_mode == 2) || (game_mode == 1)) && (points == 8)) {
      car_active = iVar2;
    }
    if ((player_count == 4) && (0xb < points)) {
      car_active = iVar2;
    }
    bVar1 = iVar2 < 4;
    iVar2 = iVar2 + 1;
  } while (bVar1);
  if (car_active != 0) {
    points = 0;
    iVar2 = 0xb8;
    do {
      if ((*(int *)(g_race_setup + iVar2) != 0) && (iVar3 = car_is_active(points), iVar3 == 1)) {
        iVar3 = player_get_points(points);
        if (((player_count == 2) ||
            ((player_count == 3 || (team_mode = is_team_mode(), team_mode != 0)))) && (iVar3 == 8))
        {
          car_active = points + 1;
        }
        if ((player_count == 4) && (0xb < iVar3)) {
          car_active = points + 1;
        }
      }
      iVar2 = iVar2 + 4;
      points = points + 1;
    } while (iVar2 < 200);
  }
  switch(game_mode) {
  case 4:
    car_active = ai_ftol_noop_wrapper(0);
    car_active = car_active + 1;
    break;
  case 5:
    car_active = car_is_active(0);
    if (car_active == 0) {
      car_active = -1;
      goto LAB_0041280c;
    }
    car_active = fx_objects_all_exploded();
    break;
  case 7:
    points = 0;
    iVar2 = get_player_count();
    if (0 < iVar2) {
      do {
        counter = (float10)ai_get_counter(points);
        if (counter > (float10)k_f_3) {
          car_active = points + 1;
          break;
        }
        points = points + 1;
        iVar2 = get_player_count();
      } while (points < iVar2);
    }
    break;
  case 8:
    car_get_mode(0,&frame.local_c,frame.local_8);
    car_active = (uint)(frame.local_c == 0) * 2 + -1;
    goto LAB_0041280c;
  case 9:
    car_active = ai_ftol_noop_wrapper(0);
    if (car_active != -1) {
LAB_004129e1:
      car_active = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    car_get_mode(0,&frame.local_c,frame.local_8);
    if (frame.local_c != 0) {
      car_active = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    counter = (float10)ai_get_counter(1);
    frame.fStack_4 = (float)counter;
    counter = (float10)ai_get_counter(0);
    if ((float10)k_f_0_9 <= (float10)frame.fStack_4 - counter) {
      car_active = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    goto LAB_004129c3;
  case 10:
    car_get_mode(0,&frame.local_c,frame.local_8);
    if (frame.local_c != 0) {
      car_active = -1;
      g_car_7ef7b0 = 0;
      goto LAB_0041280c;
    }
    counter = (float10)ai_get_counter(0);
    if (!(counter >= (float10)k_f_1_999)) {      /* NaN enters (original: test ah,1; jne) */
      if (k_f_0 <= g_course_7ef7cc) {      /* original: fld 0.0; fcomp g; test ah,0x41; jp */
        return 0;
      }
      goto LAB_004129e1;
    }
LAB_004129c3:
    car_active = 1;
    g_car_7ef7b0 = 1;
    goto LAB_0041280c;
  }
  if (car_active == 0) {
    return 0;
  }
LAB_0041280c:
  weapons_unequip_all_a();
  weapons_unequip_all_b();
  powerups_set_all_enabled(0);
  race_set_phase(RACE_PHASE_ROUND_END);
  g_intro_652df0 = 1;
  g_intro_652df4 = 700.0f;
  g_intro_652dfc = 0;
  g_game_652e00 = 0;
  if (car_active == -1) {
    camera_records_warmup(1,0);
    g_car_652df8 = 1;
    g_car_7ef7b0 = 0;
  }
  else {
    iVar2 = car_active + -1;
    camera_records_warmup(1,iVar2);
    g_car_7ef7b0 = (uint)(iVar2 == 0);
    g_car_652df8 = iVar2;
  }
  course_id = current_course_id();
  if ((course_id == COURSE_HIGHWAY) || (course_id = current_course_id(), course_id == COURSE_HIGHWAYN)) {
    course_set_lod(0);
  }
  return car_active;
}

