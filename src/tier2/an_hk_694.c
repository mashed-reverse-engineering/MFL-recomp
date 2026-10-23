#include "mfl_tier2.h"
extern char LAB_00408ef0[];
/*00408f60*/ extern int course_token_list_find_entry();
/*00408fd0*/ extern int __cdecl course_token_list_contains(undefined4 p0, undefined4 p1, undefined4 p2);
/*00409280*/ extern int fx_trajectory_solve();
/*004099f0*/ extern int player_route_progress_update();
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0042b200*/ extern int course_get_68b8b4_ptr();
/*0047ac30*/ extern int car_get_mode();
/*004b2b48*/ extern int _ftol();
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/*005dd330*/ extern float float_1_0;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddbb0*/ extern float k_f_99;
/*005ddbb4*/ extern float k_f_3000000;
/*005ddc28*/ extern float k_f_0;
/*00651ab8*/ extern undefined4 g_player_time_651ab8;
/*007ef7f8*/ extern float g_menu_wave_time;
/*008d8c20*/ extern undefined4 g_player_time_8d8c20;
/*008d8c24*/ extern undefined4 g_player_time_8d8c24;
/*008d8c28*/ extern undefined4 g_player_time_8d8c28;
/*008d8c2c*/ extern undefined4 g_player_time_8d8c2c;
/*008d8c34*/ extern float g_player_time_8d8c34;
/*008d8c38*/ extern undefined4 g_player_time_8d8c38;
/*008d8c3c*/ extern undefined4 g_player_time_8d8c3c;
/*008d8c40*/ extern undefined4 g_player_time_8d8c40;
/*008d8c44*/ extern undefined4 g_player_time_8d8c44;
/*008d8c48*/ extern undefined4 g_player_time_8d8c48;
/*008d8c5c*/ extern float g_player_time_8d8c5c;
/*008d8c80*/ extern undefined4 g_player_8d8c80;
/*008d8c88*/ extern undefined g_player_time_8d8c88;
/*008d8ca0*/ extern undefined g_player_8d8ca0;
/*008d8ca4*/ extern undefined g_player_8d8ca4;
/*008d8ca8*/ extern undefined4 g_player_8d8ca8;
/*008d8cac*/ extern undefined g_player_time_8d8cac;
/*008d8cb0*/ extern undefined g_player_time_8d8cb0;
/*008d8d48*/ extern undefined g_player_8d8d48;
/*008d8d4c*/ extern undefined g_player_8d8d4c;
/*008d8d50*/ extern undefined g_player_8d8d50;
/*008d8d54*/ extern undefined g_player_8d8d54;
/*008d8d58*/ extern undefined g_player_time_8d8d58;
/*008d8d5c*/ extern undefined g_player_time_8d8d5c;
/*008d8d60*/ extern undefined g_player_time_8d8d60;
/*008d8d64*/ extern undefined g_player_time_8d8d64;
/*008d8d68*/ extern undefined g_player_time_8d8d68;
/*008d8d6c*/ extern undefined g_player_time_8d8d6c;
/*008d8d70*/ extern undefined g_player_time_8d8d70;
/*008d8f70*/ extern undefined g_player_time_8d8f70;
/*008d8f74*/ extern undefined g_player_8d8f74;

/* constants for player_time_update (values from the code, roles inferred) */
#define PLAYER_RECORD_SIZE (0x30c) /* bytes per player record (stride) */
#define PLAYER_RECORD_WORDS (0xc3) /* same stride in 32 bit words */
#define CAR_POS_X_OFFSET (0x30)
#define CAR_POS_Y_OFFSET (0x34)
#define CAR_POS_Z_OFFSET (0x38)
#define SPHERE_QUERY_FLAGS (3) /* inferred */
#define COLLISION_RADIUS_BITS (0x3f19999a) /* float 0.6f */
#define TRAJECTORY_ARG_NEG_HALF (0xbf000000) /* float -0.5f, last argument of fx_trajectory_solve (inferred) */
#define COVERAGE_THRESHOLD_PERCENT (0x32) /* 50 percent */
#define PERCENT_SCALE (100)
#define COVERAGE_COUNT_MAX (0x1e) /* 30, cap of the coverage counter */
#define SPLIT_SLOT_COUNT (0x1e) /* 30 split slots */
#define MAX_AI_POLYGONS (0x200) /* inferred */
#define POLYGON_FLAGS_OFFSET (0xf0) /* per player byte flags, one per AI polygon (inferred) */
#define NO_ROUTE_ID (-1)
#define COVERAGE_FORCE (-1) /* coverage counter value that forces the coverage path (inferred) */

/* player_time_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per player timer update from 0x8D8C80 (stride 0x30C) when the car mode is 0 (inferred)" */

/*
 * Per player timer update, run once per frame for the player at player_index.
 * car: the car whose position (CAR_POS_*_OFFSET) is used for the collision sphere.
 * collision_world: world passed to RpCollisionWorldForAllIntersections.
 * player_index: index of the player record in the table at g_player_8d8c80 (stride PLAYER_RECORD_SIZE).
 * Returns nothing. Does work only when car_get_mode(player_index) returns 0 (inferred).
 * Route, checkpoint and split state is kept in the player record; the globals are not renamed.
 */
void __cdecl player_time_update(char *car,undefined4 collision_world,int player_index)

{
  /* tmp_int, tmp_int_b, tmp_u32 and tmp_float are reused for several values; see the block comments */
  float elapsed_time;
  float tmp_float;
  undefined4 route_id;
  int tmp_int;
  undefined4 tmp_u32;
  int tmp_int_b;
  undefined4 entry_index;
  char *visited_cursor;
  int *player;
  int loop_left;
  undefined4 *hit_flags;
  int player_offset;
  undefined4 *clear_cursor;
  /* argument block for the sphere query: sphere_x/y/z and sphere_radius form the sphere (inferred) */
  struct {
    int car_mode;
    float now_us;
    undefined1 mode_extra [4];
    int wrap_offset;
    int callback_player;
    undefined4 callback_world;
    undefined4 sphere_x;
    undefined4 sphere_y;
    undefined4 sphere_z;
    undefined4 sphere_radius;
    undefined1 pad_c[8];
    undefined4 sphere_query_flags;
  } frame;

  /* checkpoint id table from course_get_68b8b4_ptr (6 entries, inferred) */
  tmp_int = course_get_68b8b4_ptr();
  car_get_mode(player_index,&frame.car_mode,frame.mode_extra);
  if (frame.car_mode == 0) {
    /* now = g_menu_wave_time * 3000000 (microseconds if the wave time is in seconds, inferred) */
    frame.now_us = g_menu_wave_time * k_f_3000000;
    player_offset = player_index * PLAYER_RECORD_SIZE;
    player = &g_player_8d8c80 + player_index * PLAYER_RECORD_WORDS;
    /* stored timestamp in word 0 of the record, read as unsigned (inferred) */
    tmp_float = (float)*player;
    if (*player < 0) {
      tmp_float = tmp_float + k_f_4_2949673e9;
    }
    /* now is before the stored time: the clock wrapped, remember the offset (inferred) */
    if (frame.now_us - tmp_float < k_f_0) {
      frame.wrap_offset = -1 - *player;
    }
    /* time since the stored timestamp, plus the wrap offset when it wrapped (inferred) */
    elapsed_time = (frame.now_us - (float)(uint)*(int *)player);
    tmp_u32 = ((int)((!(elapsed_time < 0.0f) ? elapsed_time : ((float)(uint)frame.wrap_offset + frame.now_us)) - (float)(uint)*(int *)((char *)player + 0x1c)));
    *(undefined4 *)((char *)&g_player_time_8d8c88 + player_offset) = tmp_u32;
    car_get_mode(player_index,&frame.car_mode,frame.mode_extra);
    /* car mode 1 skips the route and checkpoint update (inferred) */
    if (frame.car_mode != 1) {
      /* sphere around the car, radius 0.6f; the callback marks the checkpoint hits */
      frame.sphere_y = *(undefined4 *)(car + CAR_POS_Y_OFFSET);
      frame.sphere_x = *(undefined4 *)(car + CAR_POS_X_OFFSET);
      frame.sphere_z = *(undefined4 *)(car + CAR_POS_Z_OFFSET);
      *(undefined4 *)((char *)&g_player_8d8ca4 + player_offset) = *(undefined4 *)((char *)&g_player_8d8ca0 + player_offset);
      frame.sphere_query_flags = SPHERE_QUERY_FLAGS;
      frame.sphere_radius = COLLISION_RADIUS_BITS;
      frame.callback_world = collision_world;
      frame.callback_player = player_index;
      RpCollisionWorldForAllIntersections(collision_world,&frame.sphere_x,&LAB_00408ef0,&frame.callback_player);
      /* hit_flags: 6 flags, cleared then set to 1 for each table entry that equals the new route id
         when the route id is exactly one past the previous one (inferred) */
      hit_flags = (undefined4 *)((char *)&g_player_8d8f74 + player_offset);
      tmp_int_b = 0;
      clear_cursor = hit_flags;
      for (loop_left = 6; loop_left != 0; loop_left = loop_left + -1) {
        *clear_cursor = 0;
        clear_cursor = clear_cursor + 1;
      }
      do {
        if ((*(int *)((char *)&g_player_8d8ca0 + player_offset) == *(int *)((char *)&g_player_8d8ca4 + player_offset) + 1) &&
           (*(int *)((char *)&g_player_8d8ca0 + player_offset) == *(int *)(tmp_int + tmp_int_b * 4))) {
          *hit_flags = 1;
        }
        tmp_int_b = tmp_int_b + 1;
        hit_flags = hit_flags + 1;
      } while (tmp_int_b < 6);
      /* route change: 8d8d50 is the committed route, 8d8d54 the pending one (inferred).
         course_token_list_contains decides whether the current route is in the committed list */
      tmp_u32 = *(undefined4 *)((char *)&g_player_8d8d50 + player_offset);
      entry_index = course_token_list_find_entry(tmp_u32);
      route_id = *(undefined4 *)((char *)&g_player_8d8ca0 + player_offset);
      tmp_int = course_token_list_contains(route_id,entry_index,tmp_u32);
      if (tmp_int == 0) {
        *(undefined4 *)((char *)&g_player_8d8d54 + player_offset) = route_id;
        tmp_u32 = course_token_list_find_entry(route_id);
        *(undefined4 *)((char *)&g_player_time_8d8d5c + player_offset) = tmp_u32;
      }
      else {
        *(undefined4 *)((char *)&g_player_8d8d50 + player_offset) = route_id;
        *(undefined4 *)((char *)&g_player_time_8d8d58 + player_offset) = entry_index;
        *(undefined4 *)((char *)&g_player_8d8d54 + player_offset) = NO_ROUTE_ID;
      }
      /* solve the trajectory from the car to the committed route, then to the pending one if set (inferred) */
      fx_trajectory_solve(player,*(undefined4 *)((char *)&g_player_8d8d50 + player_offset),
                          *(undefined4 *)((char *)&g_player_time_8d8d58 + player_offset),car + CAR_POS_X_OFFSET,
                          &g_player_time_8d8c20,TRAJECTORY_ARG_NEG_HALF);
      if (*(int *)((char *)&g_player_8d8d54 + player_offset) != NO_ROUTE_ID) {
        fx_trajectory_solve(player,*(int *)((char *)&g_player_8d8d54 + player_offset),
                            *(undefined4 *)((char *)&g_player_time_8d8d5c + player_offset),car + CAR_POS_X_OFFSET,
                            &g_player_time_8d8c48,TRAJECTORY_ARG_NEG_HALF);
      }
      /* tmp_int = slot: 1 when a pending route is set and g_player_time_8d8c34 is non-zero and below
         g_player_time_8d8c5c, else 0. The slot selects one 40 byte parameter block (10 words) from g_player_time_8d8c20 (inferred) */
      tmp_int = 0;
      if (((*(int *)((char *)&g_player_8d8d54 + player_offset) != NO_ROUTE_ID) && (g_player_time_8d8c34 != k_f_0)) &&
         (g_player_time_8d8c34 < g_player_time_8d8c5c)) {
        tmp_int = 1;
      }
      tmp_float = *(float *)&(&g_player_time_8d8c24)[tmp_int * 10];
      g_player_time_651ab8 = tmp_int;
      *(undefined4 *)((char *)&g_player_time_8d8f70 + player_offset) = (&g_player_time_8d8c2c)[tmp_int * 10];
      *(float *)((char *)&g_player_8d8d48 + player_offset) = tmp_float;
      tmp_int_b = (&g_player_time_8d8c20)[tmp_int * 10];
      *(int *)((char *)&g_player_8d8ca0 + player_offset) = tmp_int_b;
      if (tmp_int_b < 0) {
        *(undefined4 *)((char *)&g_player_8d8ca0 + player_offset) = 0;
      }
      if (tmp_float < k_f_0) {
        *(undefined4 *)((char *)&g_player_8d8d48 + player_offset) = 0;
      }
      tmp_u32 = (&g_player_time_8d8c38)[tmp_int * 10];
      *(undefined4 *)((char *)&g_player_8d8d4c + player_offset) = (&g_player_time_8d8c28)[tmp_int * 10];
      *(undefined4 *)((char *)&g_player_time_8d8d64 + player_offset) = (&g_player_time_8d8c3c)[tmp_int * 10];
      *(undefined4 *)((char *)&g_player_time_8d8d60 + player_offset) = tmp_u32;
      tmp_u32 = (&g_player_time_8d8c44)[tmp_int * 10];
      *(undefined4 *)((char *)&g_player_time_8d8d68 + player_offset) = (&g_player_time_8d8c40)[tmp_int * 10];
      *(undefined4 *)((char *)&g_player_time_8d8d6c + player_offset) = tmp_u32;
      /* advance the route progress, then mark the current route id in the per player flags (inferred) */
      player_route_progress_update(player,car);
      tmp_int = *(int *)((char *)&g_player_8d8ca0 + player_offset);
      if ((-1 < tmp_int) && (tmp_int < MAX_AI_POLYGONS)) {
        *(undefined1 *)(tmp_int + POLYGON_FLAGS_OFFSET + (char *)player) = 1;
      }
      /* coverage: while the progress fraction (8d8d48) is below float_1_0, count the AI polygons
         flagged as visited (tmp_int_b) and clear each flag as it is read */
      if (*(float *)((char *)&g_player_8d8d48 + player_offset) < float_1_0) {
        tmp_int_b = 0;
        tmp_int = course_get_ai_polygon_count();
        if (0 < tmp_int) {
          visited_cursor = &g_player_time_8d8d70 + player_offset;
          loop_left = tmp_int;
          do {
            if (*visited_cursor != '\0') {
              tmp_int_b = tmp_int_b + 1;
            }
            *visited_cursor = '\0';
            visited_cursor = visited_cursor + 1;
            loop_left = loop_left + -1;
          } while (loop_left != 0);
        }
        /* more than COVERAGE_THRESHOLD_PERCENT percent of polygons visited, or the counter is forced:
           bump the coverage counter, capped at COVERAGE_COUNT_MAX */
        if ((COVERAGE_THRESHOLD_PERCENT < (tmp_int_b * PERCENT_SCALE) / tmp_int) || ((&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] == COVERAGE_FORCE)) {
          tmp_int = (&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS];
          (&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] = tmp_int + 1;
          if (COVERAGE_COUNT_MAX < tmp_int + 1) {
            (&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] = COVERAGE_COUNT_MAX;
          }
          /* counter just reached 0 (it was COVERAGE_FORCE): store now as the record timestamp (inferred) */
          if ((&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] == 0) {
            tmp_int = ((int)(g_menu_wave_time * 3000000.0f));
            *player = tmp_int;
          }
          else {
            /* otherwise find the first zero split slot (tmp_int, up to SPLIT_SLOT_COUNT) in the split table
               at 8d8cac; player is reused as the scan cursor here */
            tmp_int = 0;
            player = (int *)((char *)&g_player_time_8d8cb0 + player_offset);
            do {
              if (player[-1] == 0) break;
              if (*player == 0) {
                tmp_int = tmp_int + 1;
                break;
              }
              if (player[1] == 0) {
                tmp_int = tmp_int + 2;
                break;
              }
              if (player[2] == 0) {
                tmp_int = tmp_int + 3;
                break;
              }
              if (player[3] == 0) {
                tmp_int = tmp_int + 4;
                break;
              }
              if (player[4] == 0) {
                tmp_int = tmp_int + 5;
                break;
              }
              if (player[5] == 0) {
                tmp_int = tmp_int + 6;
                break;
              }
              if (player[6] == 0) {
                tmp_int = tmp_int + 7;
                break;
              }
              if (player[7] == 0) {
                tmp_int = tmp_int + 8;
                break;
              }
              if (player[8] == 0) {
                tmp_int = tmp_int + 9;
                break;
              }
              tmp_int = tmp_int + 10;
              player = player + 10;
            } while (tmp_int < SPLIT_SLOT_COUNT);
            /* free slot found: the split is the elapsed time (8d8c88) minus the earlier splits, stored in that slot (inferred) */
            if (tmp_int != SPLIT_SLOT_COUNT) {
              tmp_int_b = *(int *)((char *)&g_player_time_8d8c88 + player_offset);
              if (0 < tmp_int) {
                player = (int *)((char *)&g_player_time_8d8cac + player_offset);
                loop_left = tmp_int;
                do {
                  tmp_int_b = tmp_int_b - *player;
                  player = player + 1;
                  loop_left = loop_left + -1;
                } while (loop_left != 0);
              }
              *(int *)((char *)&g_player_time_8d8cac + tmp_int * 4 + player_offset) = tmp_int_b;
            }
          }
        }
      }
      /* when 8d8d4c is above k_f_99: count visited polygons without clearing them; if coverage is below
         COVERAGE_THRESHOLD_PERCENT and the counter is 0, force the counter to COVERAGE_FORCE (inferred) */
      if (k_f_99 < *(float *)((char *)&g_player_8d8d4c + player_offset)) {
        loop_left = 0;
        tmp_int = course_get_ai_polygon_count();
        tmp_int_b = 0;
        if (0 < tmp_int) {
          do {
            if ((&g_player_time_8d8d70)[tmp_int_b + player_offset] != '\0') {
              loop_left = loop_left + 1;
            }
            tmp_int_b = tmp_int_b + 1;
          } while (tmp_int_b < tmp_int);
        }
        if (((loop_left * PERCENT_SCALE) / tmp_int < COVERAGE_THRESHOLD_PERCENT) && ((&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] == 0)) {
          (&g_player_8d8ca8)[player_index * PLAYER_RECORD_WORDS] = COVERAGE_FORCE;
        }
      }
    }
  }
  return;
}


