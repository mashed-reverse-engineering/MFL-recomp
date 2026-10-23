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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "'Waiting for autosave status' / 'status change' state machine" */

void gamesave_autosave_state_machine(float delta_time)

{
  int iVar1;
  undefined4 uVar2;
  
  switch(g_autosave_step) {
  case 0:
  case 7:
  case 8:
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
      g_autosave_step = 2;
    }
    FID_conflict__wprintf("Waiting for autosave status\n");
    return;
  case 2:
    break;
  case 3:
    message_show(0x237,0xff460000,0,0,0,0);
    gamesave_gather();
    g_gamesave_wait_timer = 0.0;
    g_autosave_step = 4;
    gamesave_set_result_code(0x8);
    return;
  case 4:
    iVar1 = gamesave_write();
    if (iVar1 == 0) {
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
    switch(g_gamesave_result_code) {
    case 0:
      message_show(0x1bc,0xff460000,0xb,0x2e,0,0);
      nop_stub();
      g_autosave_step = 0xb;
      return;
    case 1:
      uVar2 = 0x1bf;
      break;
    case 2:
      uVar2 = 0x1ba;
      break;
    case 3:
      message_show(0x24e,0xff460000,2,0x2f,0x30,0);
      g_autosave_step = 8;
      return;
    default:
      uVar2 = 0x1bb;
    }
    message_show(uVar2,0xff460000,0xb,0x2e,0,0);
    g_autosave_step = 0;
    return;
  case 6:
    message_show(0x1c5,0xff460000,2,0x2f,0x30,0);
    g_autosave_step = 7;
    return;
  case 9:
    message_show(0x280,0xff460000,2,0x2f,0x30,0);
    g_autosave_step = 10;
    return;
  default:
    return;
  }
  iVar1 = stub_return_one(0);
  if (iVar1 == 0) {
    gamesave_ui_autosave_done(1);
    return;
  }
  iVar1 = stub_return_zero();
  if (iVar1 != 0) {
    g_autosave_step = 9;
    return;
  }
  iVar1 = stub_return_one(0);
  if (iVar1 == 0) {
    gamesave_ui_autosave_done(3);
    return;
  }
  iVar1 = stub_return_one(0);
  if ((iVar1 != 0) || (iVar1 = gamesave_exists(0), iVar1 != 0)) {
    g_autosave_step = 3;
    return;
  }
  gamesave_ui_autosave_done(2);
  return;
}
