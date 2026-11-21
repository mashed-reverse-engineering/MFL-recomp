#include "mfl_tier2.h"
#include "mfl_fix.h"
/*004039b0*/ extern int attract_timer_update();
/*00405e40*/ extern int stub_return_zero();
/*004136d0*/ extern undefined __cdecl race_start_setup(undefined4 p0);
/*00430b30*/ extern int ui_get_menu_69d1ec();
/*00445180*/ extern int training_mode_update();
/*00445bc0*/ extern int message_box_update();
/*004463e0*/ extern int menu_update();
/*00475730*/ extern int get_viewport_camera();
/*004a41f0*/ extern int timer_ticks();
/*004a5b40*/ extern int __cdecl joypad_get_field_770fb4(undefined4 p0);
/*004a84d0*/ extern undefined __cdecl video_mode_name_format(undefined4 p0, undefined4 p1);
/*0050b5e0*/ extern int nop_stub();
/*0076e7f4*/ extern undefined4 g_app_state;
/*0076e834*/ extern undefined4 g_app_state_76e834;
/*0076e858*/ extern undefined4 g_app_state_76e858;
/*007ef7e8*/ extern undefined4 g_frame_ticks;

/* Race-start chunk: race_start_setup() is called with this many ticks per call (inferred) */
#define RACE_START_CHUNK_TICKS (50)

#if MFL_FIX_FIXED_STEP
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];

/* MFL_FIX_FIXED_STEP: the input record of each simulation step.  The pads are polled once per frame (controller_poll_player: previous <-
   current, then current), the steps read edges (current set, previous clear).  A frame with one step and no stepless frame before it is
   left exactly as the original.  Otherwise each step sees previous = the record the step before it saw, and the first step after
   stepless frames also gets the buttons pressed during them (so a tap shorter than a step is neither lost nor seen twice); the frame's
   own records are put back after the steps for the per-frame code (menus, pause).  ponytail: a step that writes g_player_inputs itself
   (none known) loses that write after a multi-step frame. */
#define INPUT_PLAYERS 12
static const unsigned char k_button_bytes[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                                                0x0c, 0x0d, 0x0e, 0x0f, 0x11, 0x13, 0x3f };      /* the byte fields of struct PlayerInput */
#define BUTTONS ((int)sizeof k_button_bytes)
static struct PlayerInput frame_inputs[INPUT_PLAYERS], frame_inputs_previous[INPUT_PLAYERS], step_seen[INPUT_PLAYERS];
static unsigned char latched[INPUT_PLAYERS][BUTTONS];
static int stepless_frames;

static void inputs_copy(struct PlayerInput *dst, const struct PlayerInput *src)
{
  int p;

  for (p = 0; p < INPUT_PLAYERS; p++) dst[p] = src[p];
}

void mfl_race_steps_run(void)
{
  int steps = 0 < (int)g_frame_ticks ? (int)((g_frame_ticks - 1U) / RACE_START_CHUNK_TICKS + 1) : 0;
  int step, p, b, swapped = 0;

  if (steps == 0) {      /* no step this frame: keep its presses for the next step */
    for (p = 0; p < INPUT_PLAYERS; p++)
      for (b = 0; b < BUTTONS; b++) latched[p][b] |= ((unsigned char *)&g_player_inputs[p])[k_button_bytes[b]];
    stepless_frames = 1;
    return;
  }
  for (step = 0; step < steps; step++) {
    if (step != 0 || stepless_frames != 0) {
      if (!swapped) {
        inputs_copy(frame_inputs, g_player_inputs);
        inputs_copy(frame_inputs_previous, g_player_inputs_previous);
        swapped = 1;
      }
      inputs_copy(g_player_inputs_previous, step_seen);
      inputs_copy(g_player_inputs, frame_inputs);
      for (p = 0; step == 0 && p < INPUT_PLAYERS; p++)
        for (b = 0; b < BUTTONS; b++) ((unsigned char *)&g_player_inputs[p])[k_button_bytes[b]] |= latched[p][b];
    }
    inputs_copy(step_seen, g_player_inputs);
    race_start_setup(RACE_START_CHUNK_TICKS);
  }
  if (swapped) {
    inputs_copy(g_player_inputs, frame_inputs);
    inputs_copy(g_player_inputs_previous, frame_inputs_previous);
  }
  for (p = 0; stepless_frames != 0 && p < INPUT_PLAYERS; p++)
    for (b = 0; b < BUTTONS; b++) latched[p][b] = 0;
  stepless_frames = 0;
}
#endif

/* app_state_frontend_frame */

/* "one frontend frame: game_tick, menu_update" */

/*
 * Runs one frame of the frontend state machine, selected by g_app_state.
 * Takes no parameters. Always returns 1.
 * Also stores the frame's tick delta in g_app_state_76e834 and sets the joypad latch
 * g_app_state_76e858 (see the end of the function).
 */
undefined4 app_state_frontend_frame(void)

{
  int frame_start_ticks;
  int scratch;      /* reused: menu id, race-start countdown, joypad result */
  undefined4 viewport_camera;
  undefined4 format_arg;

  frame_start_ticks = timer_ticks();
  nop_stub();
  stub_return_zero();
  /* Per-state dispatch. Each case notes which of menu_update() / message_box_update() it runs. */
  switch(g_app_state) {
  case APP_STATE_MAIN_LOOP_INIT:
    /* Attract state (inferred): start the attract timer when the menu reports id 0x21. */
    scratch = ui_get_menu_69d1ec();
    if (scratch == 0x21) {
      attract_timer_update();
    }
    break;
  case APP_STATE_COURSE_LOADED:
    goto switchD_004a2102_caseD_2;
  case APP_STATE_RACE:
    /* Training (inferred): update, then start the race in chunks of RACE_START_CHUNK_TICKS (rounded up). */
    training_mode_update();
#if MFL_FIX_FIXED_STEP
    mfl_race_steps_run();
#else
    if (0 < (int)g_frame_ticks) {
      scratch = (g_frame_ticks - 1U) / RACE_START_CHUNK_TICKS + 1;
      do {
        race_start_setup(RACE_START_CHUNK_TICKS);
        scratch = scratch + -1;
      } while (scratch != 0);
    }
#endif
    goto switchD_004a2102_caseD_2;
  case APP_STATE_MENU:
    /* Menu state (inferred): update the menu; menu id 0x20 also runs nop_stub(). Skips the second menu_update(). */
    menu_update();
    scratch = ui_get_menu_69d1ec();
    if (scratch == 0x20) {
      nop_stub();
    }
    goto switchD_004a2102_caseD_7;
  case APP_STATE_FRONTEND:
    break;
  case APP_STATE_RACE_START:
    /* Race start: same chunked race_start_setup() loop as case 3. */
#if MFL_FIX_FIXED_STEP
    mfl_race_steps_run();
#else
    if (0 < (int)g_frame_ticks) {
      scratch = (g_frame_ticks - 1U) / RACE_START_CHUNK_TICKS + 1;
      do {
        race_start_setup(RACE_START_CHUNK_TICKS);
        scratch = scratch + -1;
      } while (scratch != 0);
    }
#endif
    break;
  case APP_STATE_RACE_MESSAGE:
    goto switchD_004a2102_caseD_7;
  default:
    goto switchD_004a2102_default;
  }
  /* Shared tail for states 1, 5 and 6: menu_update(), then message_box_update() via the label below. */
#if MFL_FIX_PREFS
  mfl_menu_update_scaled();      /* menu_speed: menu_update with a scaled frame time in state 1 (the frontend); the pause menu (case 4) keeps its speed */
#else
  menu_update();
#endif
switchD_004a2102_caseD_7:
  message_box_update();
switchD_004a2102_caseD_2:
switchD_004a2102_default:
  /* Joypad 5 (inferred: player-1 pad): on the first frame it is pressed, call video_mode_name_format()
     with the viewport camera and latch g_app_state_76e858. Otherwise clear the latch. */
  scratch = joypad_get_field_770fb4(5);
  if ((scratch != 0) && (g_app_state_76e858 == 0)) {
    format_arg = 0;
    viewport_camera = get_viewport_camera(0x0);
    video_mode_name_format(viewport_camera,format_arg);
    g_app_state_76e858 = 1;
    scratch = timer_ticks();
    g_app_state_76e834 = scratch - frame_start_ticks;
    return 1;
  }
  g_app_state_76e858 = 0;
  scratch = timer_ticks();
  g_app_state_76e834 = scratch - frame_start_ticks;
  return 1;
}


