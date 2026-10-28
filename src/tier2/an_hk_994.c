#include "mfl_tier2.h"
/*00405d80*/ extern int gamesave_gather();
/*00405df0*/ extern int gamesave_write();
/*0040afc0*/ extern void __cdecl gamesave_ui_autosave_done(int result_code);      /* step = 5, result code, closes the message */
/*00405e20*/ extern int __cdecl gamesave_exists(undefined4 p0);
/*00405e40*/ extern int stub_return_zero();
/*0040af60*/ extern int gamesave_set_result_code();
/*0040b000*/ extern int gamesave_poll_status();
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*004a34a0*/ extern int stub_return_one();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*008d8bf4*/ extern undefined4 g_autosave_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c08*/ extern float g_gamesave_wait_timer;
/*008d8c10*/ extern undefined4 g_gamesave_status_poll_count;
/* gamesave_autosave_state_machine */

/* "'Waiting for autosave status' / 'status change' state machine" */

/* Per-frame autosave state machine (g_autosave_step 0..11): checks save status, shows the saving message, writes the save, waits at least 1.0s, then shows the result message (inferred). */
void gamesave_autosave_state_machine(float delta_time)

{
  int result;
  undefined4 message_id;
  
  switch(g_autosave_step) {
  case 0:
  case 7:
  case 8:
    gamesave_poll_status();
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
        nop_stub();
        message_close();
      }
      g_autosave_step = 2;
    }
    FID_conflict__wprintf("Waiting for autosave status\n");
    return;
  case 2:
    break;
  case 3:
    /* step 3: show "saving" message, gather save data, start the wait timer */
    message_show(0x237,MESSAGE_COLOUR_DARK_RED,0,0,0,0);
    gamesave_gather();
    g_gamesave_wait_timer = 0.0;
    g_autosave_step = 4;
    gamesave_set_result_code(0x8);
    return;
  case 4:
    /* step 4: write the save; close the message after the 1.0s minimum wait */
    result = gamesave_write();
    if (result == 0) {
      if (!(g_gamesave_wait_timer > float_1_0)) {
        gamesave_poll_status(*(int *)&delta_time);
      }
      else {
        message_close();
        g_autosave_step = 5;
      }
    }
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  case 5:
    /* step 5: show the message that matches the save result code */
    switch(g_gamesave_result_code) {
    case 0:
      message_show(0x1bc,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
      nop_stub();
      g_autosave_step = 0xb;
      return;
    case 1:
      message_id = 0x1bf;
      break;
    case 2:
      message_id = 0x1ba;
      break;
    case 3:
      message_show(0x24e,MESSAGE_COLOUR_DARK_RED,2,0x2f,0x30,0);
      g_autosave_step = 8;
      return;
    default:
      message_id = 0x1bb;
    }
    message_show(message_id,MESSAGE_COLOUR_DARK_RED,0xb,0x2e,0,0);
    g_autosave_step = 0;
    return;
  case 6:
    message_show(0x1c5,MESSAGE_COLOUR_DARK_RED,2,0x2f,0x30,0);
    g_autosave_step = 7;
    return;
  case 9:
    message_show(0x280,MESSAGE_COLOUR_DARK_RED,2,0x2f,0x30,0);
    g_autosave_step = 10;
    return;
  default:
    return;
  }
  result = stub_return_one(0);
  if (result == 0) {
    gamesave_ui_autosave_done(1);
    return;
  }
  result = stub_return_zero();
  if (result != 0) {
    g_autosave_step = 9;
    return;
  }
  result = stub_return_one(0);
  if (result == 0) {
    gamesave_ui_autosave_done(3);
    return;
  }
  result = stub_return_one(0);
  if ((result != 0) || (result = gamesave_exists(0), result != 0)) {
    g_autosave_step = 3;
    return;
  }
  gamesave_ui_autosave_done(2);
  return;
}
