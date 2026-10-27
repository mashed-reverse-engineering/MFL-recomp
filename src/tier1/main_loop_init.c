#include "mfl_tier2.h"
/*00430b20*/ extern int frontend_stub_return_0_b();
/*00430b50*/ extern int ui_input_mode_menu();
/*00439040*/ extern int frontend_enter_race_menu();
/*004a4250*/ extern int timer_reset_last_ticks();
/*004d4e70*/ extern RwMatrix * RwMatrixCreate();
/*0076e7f4*/ extern undefined4 g_app_state;
/*007e7f40*/ extern undefined4 g_scratch_matrix;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef804*/ extern undefined4 g_tick_number;
/*007ef808*/ extern undefined4 g_main_loop_7ef808;
/*00843e7c*/ extern undefined4 g_quit_requested;


/* Resets the quit flag and frame counters, sets app state, enters frontend menu, and creates global scratch matrix. */
undefined4 main_loop_init(void)
{
  int frontend_stub_ret;
  
  g_quit_requested = 0;
  timer_reset_last_ticks();
  g_frame_ticks = 0;
  g_tick_number = 0;
  g_main_loop_7ef808 = 0;
  g_app_state = APP_STATE_MAIN_LOOP_INIT;
  frontend_stub_ret = frontend_stub_return_0_b();
  frontend_enter_race_menu(frontend_stub_ret);
  ui_input_mode_menu();
  g_scratch_matrix = RwMatrixCreate();
  return 1;
}
