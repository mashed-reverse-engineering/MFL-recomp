#include "mfl_tier2.h"
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
    if (0 < (int)g_frame_ticks) {
      scratch = (g_frame_ticks - 1U) / RACE_START_CHUNK_TICKS + 1;
      do {
        race_start_setup(RACE_START_CHUNK_TICKS);
        scratch = scratch + -1;
      } while (scratch != 0);
    }
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
    if (0 < (int)g_frame_ticks) {
      scratch = (g_frame_ticks - 1U) / RACE_START_CHUNK_TICKS + 1;
      do {
        race_start_setup(RACE_START_CHUNK_TICKS);
        scratch = scratch + -1;
      } while (scratch != 0);
    }
    break;
  case APP_STATE_RACE_MESSAGE:
    goto switchD_004a2102_caseD_7;
  default:
    goto switchD_004a2102_default;
  }
  /* Shared tail for states 1, 5 and 6: menu_update(), then message_box_update() via the label below. */
  menu_update();
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


