#include "mfl_tier2.h"
/*0041da30*/ extern int btb_panel_set_visible_all();
/*0041dd80*/ extern undefined __cdecl time_trial_panel_update(void *p0, void *p1);
/*00434bb0*/ extern int get_event_type();
/*00494760*/ extern undefined __cdecl replay_buffer_interpolate_frame(undefined4 p0, void *p1, undefined4 p2);
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f94*/ extern undefined4 g_time_652f94;
/*00652f9c*/ extern undefined4 g_time_trial_652f9c;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];

#define FRAME_FLAGS_SET (0x20003) /* OR-ed into two frame flag words (inferred) */
#define TRACKED_EVENT_TYPES (3) /* event types 0..2 get a frame recorded (inferred) */
#define REPLAY_END_TIME_OFFSET (0x174) /* replay +0x174: end time, clamps the requested time (inferred) */
#define STEER_STRENGTH_DIVISOR (10) /* stick strength 0..255 scaled to 0..25 (inferred) */
/* time_trial_record_step */

/* Records one step of the time trial ghost. Nudges the requested time by player 0's steering,
   clamps it to the replay end time, then for event types 0..2 interpolates the ghost and
   replay frames at that time and refreshes the time trial panel.
   Parameters: unused_param is never read in this build (old header guessed "replay");
   requested_time is the time to record (inferred).
   Returns nothing. (was: "(replay, time): records one step of the time trial ghost (inferred)") */

void time_trial_record_step(undefined4 unused_param,uint requested_time)

{
  int event_type;
  undefined4 *panel_state;
  /* Two RwMatrix-shaped frames (right, flags, up, pad, at, pad, pos, pad) filled by replay_buffer_interpolate_frame: ghost at ghost_right_x, replay at replay_right_x (inferred) */
  struct {
    undefined4 *panel_replay_ptr;
    undefined4 *panel_ghost_ptr;
    undefined4 panel_flag_a;
    undefined4 panel_flag_b;
    undefined4 ghost_right_x;
    undefined4 ghost_right_y;
    undefined4 ghost_right_z;
    uint ghost_flags;
    undefined4 ghost_up_x;
    undefined4 ghost_up_y;
    undefined4 ghost_up_z;
    undefined1 ghost_up_pad[4];
    undefined4 ghost_at_x;
    undefined4 ghost_at_y;
    undefined4 ghost_at_z;
    undefined1 ghost_at_pad[4];
    undefined4 ghost_pos_x;
    undefined4 ghost_pos_y;
    undefined4 ghost_pos_z;
    undefined1 ghost_pos_pad[4];
    undefined4 replay_right_x;
    undefined4 replay_right_y;
    undefined4 replay_right_z;
    uint replay_flags;
    undefined4 replay_up_x;
    undefined4 replay_up_y;
    undefined4 replay_up_z;
    undefined1 replay_up_pad[4];
    undefined4 replay_at_x;
    undefined4 replay_at_y;
    undefined4 replay_at_z;
    undefined1 replay_at_pad[4];
    undefined4 replay_pos_x;
    undefined4 replay_pos_y;
    undefined4 replay_pos_z;
    undefined4 replay_pos_pad;
  } frame;
  
  /* Time trial running: shift the requested time by player 0's steering and by the bytes
     at PlayerInput +0x41 and +0x42 (meaning unknown) */
  if (g_time_trial_652f9c != 0) {
    requested_time = (byte)(*(byte *)((char *)&g_player_inputs[0] + 66)) + g_time_652f94 +
              ((g_player_inputs[0].right / STEER_STRENGTH_DIVISOR - g_player_inputs[0].left / STEER_STRENGTH_DIVISOR) -
              (uint)(byte)(*(byte *)((char *)&g_player_inputs[0] + 65)));
  }
  /* Store the time, then never go past the replay end time */
  g_time_652f94 = requested_time;
  if ((g_replay_652f88 != 0) &&
     (g_time_652f94 = requested_time, *(uint *)(g_replay_652f88 + REPLAY_END_TIME_OFFSET) < requested_time)) {
    g_time_652f94 = *(uint *)(g_replay_652f88 + REPLAY_END_TIME_OFFSET);
  }
  /* Only event types 0..2 record and display a frame (inferred) */
  event_type = get_event_type();
  if ((-1 < event_type) && (event_type < TRACKED_EVENT_TYPES)) {
    frame.panel_ghost_ptr = &frame.ghost_right_x;
    frame.replay_flags = frame.replay_flags | FRAME_FLAGS_SET;
    frame.ghost_flags = frame.ghost_flags | FRAME_FLAGS_SET;
    frame.panel_replay_ptr = &frame.replay_right_x;
    frame.panel_flag_a = 1;
    frame.panel_flag_b = 0;
    frame.replay_at_z = FLOAT_ONE_BITS;
    frame.replay_up_y = FLOAT_ONE_BITS;
    frame.replay_right_x = FLOAT_ONE_BITS;
    frame.replay_up_x = 0;
    frame.replay_right_z = 0;
    frame.replay_right_y = 0;
    frame.replay_at_y = 0;
    frame.replay_at_x = 0;
    frame.replay_up_z = 0;
    frame.replay_pos_z = 0;
    frame.replay_pos_y = 0;
    frame.replay_pos_x = 0;
    frame.ghost_at_z = FLOAT_ONE_BITS;
    frame.ghost_up_y = FLOAT_ONE_BITS;
    frame.ghost_right_x = FLOAT_ONE_BITS;
    frame.ghost_up_x = 0;
    frame.ghost_right_z = 0;
    frame.ghost_right_y = 0;
    frame.ghost_at_y = 0;
    frame.ghost_at_x = 0;
    frame.ghost_up_z = 0;
    frame.ghost_pos_z = 0;
    frame.ghost_pos_y = 0;
    frame.ghost_pos_x = 0;
    /* Interpolate the ghost and replay frames at the stored time, then update the panel */
    if (g_ghost_652f84 != 0) {
      replay_buffer_interpolate_frame(g_ghost_652f84,&frame.ghost_right_x,g_time_652f94);
    }
    if (g_replay_652f88 != 0) {
      replay_buffer_interpolate_frame(g_replay_652f88,&frame.replay_right_x,g_time_652f94);
    }
    panel_state = &frame.panel_flag_a;
    btb_panel_set_visible_all(&frame.panel_flag_a);
    time_trial_panel_update(&frame.panel_replay_ptr,panel_state);
  }
  return;
}


