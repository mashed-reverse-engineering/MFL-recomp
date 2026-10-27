#include "mfl_tier2.h"
/*00409f30*/ extern double player_progress_b();
/*0040fbf0*/ extern int race_get_focus_player();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005ddb18*/ extern float float_0_8;
/*005ddea0*/ extern float k_f_20;
/*008c7e30*/ extern undefined4 leader_index;
/*008c7e40*/ extern undefined4 distance_to_leader;
/*008c7e44*/ extern undefined4 g_distances_update_8c7e44;
/*008c7e48*/ extern undefined4 g_distances_update_8c7e48;
/*008c7e4c*/ extern undefined4 g_distances_update_8c7e4c;
/*008c7e60*/ extern undefined4 g_distances_8c7e60;
#define PLAYER_SLOT_COUNT (4)
#define MATRIX_POS_X_OFFSET (0x30)
#define MATRIX_POS_Z_OFFSET (0x38)
/* distances_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* per-player horizontal distance to the leader * 0.8 -> distance_to_leader[4] */
/*
 * Recomputes distance_to_leader[] for every player slot (no parameters, no return value).
 * The two focus players are compared by lap progress (progress is a percent of a lap, inferred from
 * the k_f_20 / k_f_80 / k_f_100 constants). The car ahead becomes front_player; the car behind is back_player.
 * Leader rule: front_player unless its car_get_mode() is non-zero, then back_player if that one's mode is 0.
 * Writes leader_index and g_distances_8c7e60 (the other focus car). If the leader is in use and active,
 * each in-use, active slot gets 0.8 * XZ distance (y ignored) to the leader's matrix position.
 */

void distances_update(void)

{
  int back_mode;
  int other_player;
  int back_player;
  int leader_player;
  double progress_value;
  RwReal horizontal_distance;
  struct {
    int player_index;
    int front_player;
    float progress_a;
    float progress_b;
    char *matrix_ptr;
    int front_mode;
    int mode_a;
    undefined1 mode_a_extra[4];
    undefined1 mode_b_extra[4];
    float leader_pos_x;
    undefined1 pad_14[4];
    float leader_pos_z;
    RwV3d offset_to_leader;
  } frame;

  distance_to_leader = 0;
  g_distances_update_8c7e44 = 0;
  g_distances_update_8c7e48 = 0;
  g_distances_update_8c7e4c = 0;
  frame.player_index = 4;
  /* the two focus players and their lap progress */
  race_get_focus_player(&frame.player_index,&frame.front_player);
  progress_value = (double)player_progress_b(frame.player_index);
  frame.progress_a = (float)progress_value;
  progress_value = (double)player_progress_b(frame.front_player);
  frame.progress_b = (float)progress_value;
  /* start/finish wrap (inferred): if one car is just past the line (< 20) and the other is near the end (> 80),
     subtract a full lap (100) from the one near the end so it compares as behind */
  if ((!(frame.progress_a > k_f_80)) || (!(k_f_20 > frame.progress_b))) {
    if ((k_f_80 < frame.progress_b) && (frame.progress_a < k_f_20)) {
      frame.progress_b = frame.progress_b - k_f_100;
    }
  }
  else {
    frame.progress_a = frame.progress_a - k_f_100;
  }
  /* car modes of both focus players */
  car_get_mode(frame.player_index,&frame.mode_a,frame.mode_a_extra);
  car_get_mode(frame.front_player,&frame.front_mode,frame.mode_b_extra);
  /* front_player must end up as the car with more progress (front_mode follows it); back_player is the other one.
     back_player is also reused below as the return value of race_setup_slot_used / car_is_active */
  back_player = frame.player_index;
  back_mode = frame.mode_a;
  if (frame.progress_b < frame.progress_a) {
    back_player = frame.front_player;
    frame.front_player = frame.player_index;
    back_mode = frame.front_mode;
    frame.front_mode = frame.mode_a;
  }
  /* leader choice (the front car unless its mode is non-zero; then the back car if its mode is 0).
     The leader must be in use and active before anything is measured from it */
  other_player = back_player;
  leader_player = frame.front_player;
  if ((((frame.front_mode == 0) || (other_player = frame.front_player, leader_player = back_player, back_mode == 0)) &&
      (leader_index = leader_player, g_distances_8c7e60 = other_player, back_player = race_setup_slot_used(leader_player),
      back_player != 0)) && (back_player = car_is_active(leader_player), back_player == 1)) {
    car_current_matrix(&frame.matrix_ptr,leader_player);
    frame.leader_pos_x = *(float *)(frame.matrix_ptr + MATRIX_POS_X_OFFSET);
    frame.leader_pos_z = *(float *)(frame.matrix_ptr + MATRIX_POS_Z_OFFSET);
    /* every slot that is in use and active: 0.8 * XZ distance to the leader */
    frame.player_index = 0;
    do {
      back_player = race_setup_slot_used(frame.player_index);
      if ((back_player != 0) && (back_player = car_is_active(frame.player_index), back_player == 1)) {
        car_current_matrix(&frame.matrix_ptr,frame.player_index);
        frame.offset_to_leader.x = frame.leader_pos_x - *(float *)(frame.matrix_ptr + MATRIX_POS_X_OFFSET);
        frame.offset_to_leader.y = 0.0;
        frame.offset_to_leader.z = frame.leader_pos_z - *(float *)(frame.matrix_ptr + MATRIX_POS_Z_OFFSET);
        horizontal_distance = RwV3dLength(&frame.offset_to_leader);
        *(float *)(((undefined *)&distance_to_leader) + frame.player_index * 4) = horizontal_distance * float_0_8;
      }
      frame.player_index = frame.player_index + 1;
    } while (frame.player_index < PLAYER_SLOT_COUNT);
  }
  return;
}


