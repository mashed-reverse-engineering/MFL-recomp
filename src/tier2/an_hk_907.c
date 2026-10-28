#include "mfl_tier2.h"
/*00405cf0*/ extern int gamesave_load();
/*00405d20*/ extern int gamesave_apply();
/*00405d80*/ extern int gamesave_gather();
/*00405df0*/ extern int gamesave_write();
/*00405e20*/ extern int __cdecl gamesave_exists(undefined4 p0);
/*00405e40*/ extern int stub_return_zero();
/*0040b000*/ extern int gamesave_poll_status(float p0);
/*0040af70*/ extern void gamesave_ui_load_done(int p0);
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*004a34a0*/ extern int stub_return_one();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*008d8be8*/ extern undefined4 g_gamesave_format_step;
/*008d8bec*/ extern undefined4 g_gamesave_load_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c08*/ extern float g_gamesave_wait_timer;
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;
/*008d8c10*/ extern undefined4 g_gamesave_status_poll_count;
/*008d8c14*/ extern undefined4 g_gamesave_load_failed;
/* gamesave_load_menu_state */

/* state machine for the boot-time save check (memory-card style states from the console builds) */

/* Per-frame state machine of the boot-time save check: waits for boot status, picks the prompt (no card / unformatted / load / write), shows the message, runs load or write and applies the result; steps are g_gamesave_format_step (inferred). */
void gamesave_load_menu_state(float delta_time)

{
  int result;
  undefined4 message_id;
  char *log_text;
  
  switch(g_gamesave_format_step) {
  case 0:
  case 6:
  case 8:
  case 0xb:
    gamesave_poll_status(delta_time);
    return;
  case 1:
    result = stub_return_zero();
    if (result == 0) {
      g_gamesave_status_poll_count = g_gamesave_status_poll_count + 1;
    }
    if (1 < (int)g_gamesave_status_poll_count) {
      result = stub_return_zero();
      if (result != 0) {
        FID_conflict__wprintf("status change\n");
        message_close();
      }
      g_gamesave_format_step = 2;
    }
    FID_conflict__wprintf("Waiting for boot status\n");
    return;
  case 2:
    result = stub_return_one(0);
    if (result == 0) {
      g_gamesave_format_step = 4;
      return;
    }
    result = stub_return_one(0);
    if (result == 0) {
      g_gamesave_format_step = 10;
      return;
    }
    result = gamesave_exists(0);
    if (result != 0) {
      g_gamesave_format_step = 9;
      return;
    }
    result = stub_return_one(0);
    g_gamesave_format_step = (-(uint)(result != 0) & 5) + 5;
    break;
  case 3:
    if (g_gamesave_result_code == 3) {
      message_show(0x24e,MESSAGE_COLOUR_DARK_RED,2,0x2f,0x30,0);
      g_gamesave_format_step = 6;
      return;
    }
    if (g_gamesave_result_code == 5) {
      message_show(0x1c0,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
      g_gamesave_format_step = 0;
      g_gamesave_load_failed = 1;
      return;
    }
    break;
  case 4:
    message_id = 0x281;
    goto LAB_0040b1cc;
  case 5:
    message_id = 0x289;
LAB_0040b1cc:
    message_show(message_id,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
    g_gamesave_format_step = 0;
    g_gamesave_flag_c0c = 0;
    return;
  case 7:
    message_show(0x1c5,MESSAGE_COLOUR_DARK_RED,2,0x2f,0x30,0);
    g_gamesave_format_step = 8;
    return;
  case 9:
    message_show(0x236,MESSAGE_COLOUR_DARK_RED,0,0,0,0);
    g_gamesave_format_step = 0xd;
    g_gamesave_wait_timer = 0.0;
    nop_stub();
    g_gamesave_load_failed = 0;
    return;
  case 10:
    message_show(0x2ae,MESSAGE_COLOUR_DARK_RED,2,0x290,0x291,0);
    g_gamesave_format_step = 0xb;
    return;
  case 0xc:
    result = stub_return_one(0);
    if (result == 0) {
      gamesave_ui_load_done(3);
      return;
    }
    message_show(0x238,MESSAGE_COLOUR_DARK_RED,0,0,0,0);
    g_gamesave_format_step = 0xe;
    g_gamesave_wait_timer = 0.0;
    gamesave_gather(*(int *)&delta_time);
    return;
  case 0xd:
    result = gamesave_load();
    if (result == 0) {
      if (float_1_0 < g_gamesave_wait_timer) {
        message_close();
        g_gamesave_format_step = 0xf;
        goto LAB_0040b376;
      }
LAB_0040b27c:
      gamesave_poll_status(*(int *)&delta_time);
      log_text = "Waiting for time to elapse\n";
    }
    else {
      log_text = "Waiting for load to complete\n";
    }
    goto LAB_0040b36e;
  case 0xe:
    result = gamesave_write();
    if (result == 0) {
      if (float_1_0 < g_gamesave_wait_timer) {
        message_close();
        g_gamesave_format_step = 0x10;
        goto LAB_0040b376;
      }
      goto LAB_0040b27c;
    }
    log_text = "Waiting for save to complete\n";
LAB_0040b36e:
    FID_conflict__wprintf(log_text);
LAB_0040b376:
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  case 0xf:
    message_show(0x1c1,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
    gamesave_apply();
    g_gamesave_format_step = 0x11;
    return;
  case 0x10:
    message_show(0x1bc,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
    nop_stub();
    g_gamesave_format_step = 0x11;
    g_gamesave_flag_c0c = 1;
    return;
  }
  return;
}


