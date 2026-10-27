// cluster-entries: gamesave_load_state_machine
/* Installed cluster gamesave_load_state_machine (tools/cluster_install.py from output/cluster_pilot_kb/gamesave_load_state_machine.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00405cf0*/ extern int gamesave_load();
/*00405d20*/ extern int gamesave_apply();
/*00405e20*/ extern int __cdecl gamesave_exists(undefined4 p0);
/*00405e40*/ extern int stub_return_zero();
/*0040af60*/ extern int gamesave_set_result_code();
/*0040b000*/ extern int gamesave_poll_status();
/*0040af70*/ extern void gamesave_ui_load_done(int result_code);
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*004a34a0*/ extern int stub_return_one();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8be8*/ extern undefined4 g_gamesave_format_step;
/*008d8bec*/ extern undefined4 g_gamesave_load_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c08*/ extern float g_gamesave_wait_timer;
/*008d8c10*/ extern undefined4 g_gamesave_status_poll_count;
/*008d8c14*/ extern undefined4 g_gamesave_load_failed;
/* gamesave_load_state_machine */

/* "'Waiting for load status' state machine" */

void gamesave_load_state_machine(float delta_time)

{
  int iVar1;
  char *_Format;
  
  switch(g_gamesave_load_step) {
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
      message_show(0x21a,0xff460000,2,0x2f,0x30,0);
      g_gamesave_load_step = 7;
    }
    FID_conflict__wprintf("Waiting for load status\n");
    return;
  case 2:
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_load_done(1);
      return;
    }
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_load_done(3);
      return;
    }
    iVar1 = gamesave_exists(0);
    if (iVar1 != 0) {
      g_gamesave_load_step = 3;
      return;
    }
    gamesave_ui_load_done(4);
    return;
  case 3:
    g_gamesave_load_failed = 0;
    message_show(0x236,0xff460000,0,0,0,0);
    g_gamesave_wait_timer = 0.0;
    g_gamesave_load_step = 4;
    gamesave_set_result_code(5);
    nop_stub(*(int *)&delta_time);
    return;
  case 4:
    break;
  case 5:
    switch(g_gamesave_result_code) {
    case 0:
      message_show(0x1c1,0xff460000,0xb,0x2e,0,0);
      gamesave_apply();
      g_gamesave_load_step = 6;
      return;
    case 1:
      message_show(0x1bf,0xff460000,0xb,0x2e,0,0);
      g_gamesave_load_step = 0;
      return;
    default:
      message_show(0x1c0,0xff460000,0xb,0x2e,0,0);
      g_gamesave_load_step = 0;
      g_gamesave_load_failed = 1;
      break;
    case 3:
    case 4:
      message_show(0x1be,0xff460000,0xb,0x2e,0,0);
      g_gamesave_load_step = 0;
      return;
    }
  case 6:
    g_gamesave_operation = 0;
  default:
    return;
  }
  iVar1 = gamesave_load();
  if (iVar1 == 0) {
    if (float_1_0 < g_gamesave_wait_timer) {
      message_close();
      g_gamesave_load_step = 5;
      goto LAB_0040b8d2;
    }
    gamesave_poll_status(*(int *)&delta_time);
    _Format = "Waiting for time to elapse\n";
  }
  else {
    _Format = "Waiting for load to complete\n";
  }
  FID_conflict__wprintf(_Format);
LAB_0040b8d2:
  g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
  return;
}

