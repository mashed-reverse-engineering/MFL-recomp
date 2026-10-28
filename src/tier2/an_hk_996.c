#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*0040ae50*/ extern int gamesave_request_format();
/*0040ae80*/ extern int loading_start_log();
/*0040aed0*/ extern int gamesave_request_save_b();
/*0040af20*/ extern int autosave_request();
/*0040af60*/ extern int gamesave_set_result_code();
/*0040afe0*/ extern int gamesave_ui_format_done(int result_code);      /* step = 5, result code, closes the message */
/*0040b000*/ extern int gamesave_poll_status();
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*004a34a0*/ extern int stub_return_one();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*008d8be0*/ extern undefined4 g_gamesave_previous_operation;
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8bf8*/ extern undefined4 g_gamesave_delete_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c08*/ extern float g_gamesave_wait_timer;
/*008d8c10*/ extern undefined4 g_gamesave_status_poll_count;
/* gamesave_format_state_machine */

/* "'process format %d' state machine" */

void gamesave_format_state_machine(float delta_time)

{
  int iVar1;
  
  FID_conflict__wprintf("process format %d\n",g_gamesave_delete_step);
  switch(g_gamesave_delete_step) {
  case 0:
    gamesave_poll_status();
    return;
  case 1:
    iVar1 = stub_return_zero();
    if (iVar1 == 0) {
      g_gamesave_status_poll_count = g_gamesave_status_poll_count + 1;
    }
    if (1 < (int)g_gamesave_status_poll_count) {
      iVar1 = stub_return_zero();
      if (iVar1 != 0) {
        FID_conflict__wprintf("status change\n");
        nop_stub();
        message_close();
      }
      g_gamesave_delete_step = 2;
    }
    FID_conflict__wprintf("Waiting for format status\n");
    return;
  case 2:
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_format_done(1);
      return;
    }
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      g_gamesave_delete_step = 3;
      return;
    }
    gamesave_ui_format_done(9);
    return;
  case 3:
    message_close();
    message_show(0x1c6,MESSAGE_COLOUR_DARK_RED,0,0,0,0);
    nop_stub();
    g_gamesave_wait_timer = 0.0;
    g_gamesave_delete_step = 4;
    gamesave_set_result_code(0x9);
    return;
  case 4:
    break;
  case 5:
    if (g_gamesave_result_code == 0) {
      message_show(0x1c8,MESSAGE_COLOUR_DARK_RED,0,0,0,0);
      nop_stub();
      g_gamesave_wait_timer = 0.0;
      g_gamesave_delete_step = 8;
      return;
    }
    message_show(0x1c9,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
    g_gamesave_delete_step = 0;
    return;
  default:
    return;
  case 8:
    if (float_1_0 < g_gamesave_wait_timer) {
      message_close();
      switch(g_gamesave_previous_operation) {
      case 1:
        gamesave_request_format();
        g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
        return;
      case 2:
        loading_start_log();
        g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
        return;
      case 3:
        gamesave_request_save_b();
        g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
        return;
      case 4:
        autosave_request();
        g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
        return;
      default:
        g_gamesave_operation = 0;
      }
    }
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  }
  iVar1 = stub_return_zero();
  if (iVar1 != 0) {
    FID_conflict__wprintf("Waiting for format to complete\n");
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  }
  if (!(g_gamesave_wait_timer > float_1_0)) {
    gamesave_poll_status(*(int *)&delta_time);
    FID_conflict__wprintf("Waiting for time to elapse\n");
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  }
  message_close();
  g_gamesave_delete_step = 5;
  g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
  return;
}


