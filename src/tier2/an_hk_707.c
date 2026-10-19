#include "mfl_tier2.h"
typedef int CourseId;
/*00408ea0*/ extern int player_record_get_8d8ca0();
/*0040fdc0*/ extern int race_get_phase();
/*0040ff60*/ extern int race_get_phase_timer();
/*00416a70*/ extern int ai_bots_reset();
/* ai_drive_a takes the same four stack arguments as ai_drive_b / ai_drive_c: the original pushes them once before the game_mode branch (push 100.0f, input, bot, route) and the call to ai_drive_a reuses them */
/*00418dd0*/ extern undefined __cdecl ai_drive_a(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004195b0*/ extern undefined __cdecl ai_drive_b(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00419d00*/ extern int ai_bot_stage_state_update();
/*0041a280*/ extern int ai_bot_course_0x21_target_check();
/*0041ad20*/ extern undefined __cdecl ai_drive_c(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0042b110*/ extern int current_course_id();
/*00434bb0*/ extern int get_event_type();
/*00437f10*/ extern int training_lesson_get();
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*007ef7f0*/ extern undefined4 g_tick_count;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0250*/ extern undefined4 g_ai_editor_route_set;
/*007f0254*/ extern undefined4 g_ai_editor_route_index;
/*007f0284*/ extern struct AiData g_ai_data;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008014ac*/ extern char g_ai_data_routes_4644_[];
/*00800ea0*/ extern char g_ai_data_routes_3096_[];
/*00800894*/ extern char g_ai_data_routes_1548_[];
/*00800288*/ extern char g_ai_data_routes_0_[];
/* ai_bot_update */
#define COURSE_WARZONE (33)
/* (inferred) AiRoute slot stride: 4 sets x 3 slots fill the 6192-byte routes array */
#define AI_ROUTE_SLOT_SIZE (0x204)
/* a route slot with fewer points than this is skipped and slot 0 is used */
#define ROUTE_MIN_POINTS (4)
/* bit pattern of 100.0f, 4th argument of ai_drive_a / ai_drive_b / ai_drive_c (meaning inferred) */
#define AI_DRIVE_PARAM_100F (0x42c80000)
/* (inferred) exclusive bounds of the warzone brake windows, compared with player_record_get_8d8ca0() */
#define WARZONE_BRAKE_WINDOW_A_LOW (0x37)
#define WARZONE_BRAKE_WINDOW_A_HIGH (0x44)
#define WARZONE_BRAKE_WINDOW_B_LOW (0x58)
#define WARZONE_BRAKE_WINDOW_B_HIGH (0x5f)
/* (inferred) get_event_type() value for training, the only event that calls training_lesson_get() */
#define EVENT_TYPE_TRAINING (0x11)

/* Per-frame AI update for one bot. slot_index is the player slot, which is also the index into
   g_ai_bots. Clears the bot's input bytes, then in a normal race picks a route (stage = route set,
   stage_counter = slot), drives with ai_drive_a / _b / _c, and while cooldown_timer runs replays the
   inputs saved on the previous frame. In race phase 5 (inferred: race start) it only holds the
   throttle until a start time derived from start_random. The phase 5 path also resets the race
   clock and the bots. unused_arg is never read. */

void __cdecl ai_bot_update(int slot_index,char *unused_arg)

{
  struct PlayerInput *input_ptr; /* same pointer as input, used for the per-field writes */
  byte saved_right; /* this frame's right / accelerate / brake, also reused by the cooldown replay */
  byte saved_accelerate;
  byte saved_brake;
  int input_index; /* g_player_slots[slot_index].controller, index into g_player_inputs */
  int scratch; /* route address in the race path; reused as a temp for the record value, the cooldown timer and the start value */
  int stage_or_timer; /* stage in the race path, phase timer in the start path */
  int event_type;
  CourseId course_id;
  PlayerInput *input;
  
  input_index = g_player_slots[slot_index].controller;
  input = g_player_inputs + input_index;
  input_ptr = &g_player_inputs[input_index];
  input_ptr->accelerate = 0;
  input_ptr->brake = 0;
  input->left = 0;
  input_ptr->right = 0;
  input_ptr->fire = 0;
  input_ptr->field_0x6 = 0;
  scratch = race_get_phase();
  if (scratch != 5) {
    /* normal race: choose the route, then drive */
    ai_bot_stage_state_update(slot_index);
    scratch = ((int)&g_ai_data_routes_0_);
    if (2 < g_ai_bots[slot_index].stage_counter) {
      g_ai_bots[slot_index].stage_counter = 0;
    }
    /* route selection: stage 0..3 picks the route set (3 slots of AI_ROUTE_SLOT_SIZE each); a slot
       with fewer than ROUTE_MIN_POINTS points falls back to slot 0 of set 0 */
    stage_or_timer = g_ai_bots[slot_index].stage;
    if (stage_or_timer == 0) {
      if (*(int *)(g_ai_data.routes + g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE + 0x200) < ROUTE_MIN_POINTS) {
        g_ai_bots[slot_index].stage_counter = 0;
      }
      scratch = g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE + ((int)&g_ai_data_routes_0_);
    }
    if (stage_or_timer == 2) {
      if (*(int *)(g_ai_data.routes + g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE + 0xe18) < ROUTE_MIN_POINTS) {
        g_ai_bots[slot_index].stage_counter = 0;
      }
      scratch = g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE;
      if (*(int *)(g_ai_data.routes + scratch + 0xe18) < ROUTE_MIN_POINTS) {
        scratch = ((int)&g_ai_data_routes_0_);
      }
      else {
        scratch = scratch + ((int)&g_ai_data_routes_3096_);
      }
    }
    if (stage_or_timer == 1) {
      if (*(int *)(g_ai_data.routes + g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE + 0x80c) < ROUTE_MIN_POINTS) {
        g_ai_bots[slot_index].stage_counter = 0;
      }
      scratch = g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE;
      if (*(int *)(g_ai_data.routes + scratch + 0x80c) < ROUTE_MIN_POINTS) {
        scratch = ((int)&g_ai_data_routes_0_);
      }
      else {
        scratch = scratch + ((int)&g_ai_data_routes_1548_);
      }
    }
    if (stage_or_timer == 3) {
      if (*(int *)(g_ai_data.routes + g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE + 0x1424) < ROUTE_MIN_POINTS) {
        g_ai_bots[slot_index].stage_counter = 0;
      }
      scratch = g_ai_bots[slot_index].stage_counter * AI_ROUTE_SLOT_SIZE;
      if (*(int *)(g_ai_data.routes + scratch + 0x1424) < ROUTE_MIN_POINTS) {
        scratch = ((int)&g_ai_data_routes_0_);
      }
      else {
        scratch = scratch + ((int)&g_ai_data_routes_4644_);
      }
    }
    /* route editor override: player slot 3 extra == 1 uses the editor's set and index (inferred) */
    if (g_player_slots[3].extra == 1) {
      switch(g_ai_editor_route_set) {
      default:
        scratch = g_ai_editor_route_index * AI_ROUTE_SLOT_SIZE + ((int)&g_ai_data_routes_0_);
        break;
      case 1:
        scratch = g_ai_editor_route_index * AI_ROUTE_SLOT_SIZE + ((int)&g_ai_data_routes_1548_);
        break;
      case 2:
        scratch = g_ai_editor_route_index * AI_ROUTE_SLOT_SIZE + ((int)&g_ai_data_routes_3096_);
        break;
      case 3:
        scratch = g_ai_editor_route_index * AI_ROUTE_SLOT_SIZE + ((int)&g_ai_data_routes_4644_);
      }
    }
    /* drive: game modes 4 and 9 use ai_drive_b, mode 8 uses ai_drive_c, others use ai_drive_a */
    if ((game_mode == 4) || (game_mode == 9)) {
      ai_drive_b(scratch,slot_index,(undefined4)input,AI_DRIVE_PARAM_100F);
    }
    else if (game_mode == 8) {
      ai_drive_c(scratch,slot_index,(undefined4)input,AI_DRIVE_PARAM_100F);
      course_id = current_course_id();
      /* warzone: release the brake while the player record value is inside either window */
      if (course_id == COURSE_WARZONE) {
        scratch = player_record_get_8d8ca0(slot_index);
        if ((scratch < WARZONE_BRAKE_WINDOW_A_HIGH) && (WARZONE_BRAKE_WINDOW_A_LOW < scratch)) {
          input_ptr->brake = 0;
        }
        if ((scratch < WARZONE_BRAKE_WINDOW_B_HIGH) && (WARZONE_BRAKE_WINDOW_B_LOW < scratch)) {
          input_ptr->brake = 0;
        }
      }
    }
    else {
      ai_drive_a(scratch,slot_index,(undefined4)input,AI_DRIVE_PARAM_100F);
    }
    ai_bot_course_0x21_target_check(slot_index,input);
    /* cooldown: count cooldown_timer down by g_tick_count and replay the saved inputs */
    if (g_ai_bots[slot_index].cooldown_timer != 0) {
      scratch =g_ai_bots[slot_index].cooldown_timer - g_tick_count;
      g_ai_bots[slot_index].cooldown_timer = scratch;
      if (scratch < 0) {
        g_ai_bots[slot_index].cooldown_timer = 0;
      }
      saved_right = g_ai_bots[slot_index].field_0x54;
      saved_accelerate = g_ai_bots[slot_index].field_0x58;
      input->left = g_ai_bots[slot_index].field_0x50;
      saved_brake = g_ai_bots[slot_index].field_0x5c;
      input_ptr->right = saved_right;
      input_ptr->accelerate = saved_accelerate;
      input_ptr->brake = saved_brake;
    }
    /* store this frame's inputs for the next cooldown replay */
    saved_right = input_ptr->right;
    saved_accelerate = input_ptr->accelerate;
    *(uint *)&g_ai_bots[slot_index].field_0x50 = (uint)input->left;
    saved_brake = input_ptr->brake;
    *(uint *)&g_ai_bots[slot_index].field_0x54 = (uint)saved_right;
    *(uint *)&g_ai_bots[slot_index].field_0x58 = (uint)saved_accelerate;
    *(uint *)&g_ai_bots[slot_index].field_0x5c = (uint)saved_brake;
    return;
  }
  /* race start (phase 5): reset the race clock and bots, then hold the throttle until the phase timer
     passes the bot's start time. Training events (EVENT_TYPE_TRAINING) use 0x4a - start value instead
     of start value + 0x40 (both inferred) */
  g_race_clock_b = 0;
  ai_bots_reset();
  stage_or_timer = race_get_phase_timer();
  scratch = g_ai_bots[slot_index].start_random;
  event_type = get_event_type();
  if (event_type == EVENT_TYPE_TRAINING) {
    scratch = -1;
    event_type = training_lesson_get();
    if (event_type != 5) {
LAB_0041b575:
      if ((0x4a - scratch) * 100 <= stage_or_timer) {
        return;
      }
      input_ptr->accelerate = 0xff;
      return;
    }
    scratch = 9;
  }
  else if (scratch < 0) goto LAB_0041b575;
  if ((scratch + 0x40) * 100 <= stage_or_timer) {
    return;
  }
  input_ptr->accelerate = 0xff;
  return;
}


