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

void gamesave_load_menu_state(float delta_time)

{
  int iVar1;
  undefined4 uVar2;
  char *_Format;
  
  switch(g_gamesave_format_step) {
  case 0:
  case 6:
  case 8:
  case 0xb:
    gamesave_poll_status(delta_time);
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
        message_close();
      }
      g_gamesave_format_step = 2;
    }
    FID_conflict__wprintf("Waiting for boot status\n");
    return;
  case 2:
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      g_gamesave_format_step = 4;
      return;
    }
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      g_gamesave_format_step = 10;
      return;
    }
    iVar1 = gamesave_exists(0);
    if (iVar1 != 0) {
      g_gamesave_format_step = 9;
      return;
    }
    iVar1 = stub_return_one(0);
    g_gamesave_format_step = (-(uint)(iVar1 != 0) & 5) + 5;
    break;
  case 3:
    if (g_gamesave_result_code == 3) {
      message_show(0x24e,0xff460000,2,0x2f,0x30,0);
      g_gamesave_format_step = 6;
      return;
    }
    if (g_gamesave_result_code == 5) {
      message_show(0x1c0,0xff460000,0xb,0x2e,0,0);
      g_gamesave_format_step = 0;
      g_gamesave_load_failed = 1;
      return;
    }
    break;
  case 4:
    uVar2 = 0x281;
    goto LAB_0040b1cc;
  case 5:
    uVar2 = 0x289;
LAB_0040b1cc:
    message_show(uVar2,0xff460000,0xb,0x2e,0,0);
    g_gamesave_format_step = 0;
    g_gamesave_flag_c0c = 0;
    return;
  case 7:
    message_show(0x1c5,0xff460000,2,0x2f,0x30,0);
    g_gamesave_format_step = 8;
    return;
  case 9:
    message_show(0x236,0xff460000,0,0,0,0);
    g_gamesave_format_step = 0xd;
    g_gamesave_wait_timer = 0.0;
    nop_stub();
    g_gamesave_load_failed = 0;
    return;
  case 10:
    message_show(0x2ae,0xff460000,2,0x290,0x291,0);
    g_gamesave_format_step = 0xb;
    return;
  case 0xc:
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_load_done(3);
      return;
    }
    message_show(0x238,0xff460000,0,0,0,0);
    g_gamesave_format_step = 0xe;
    g_gamesave_wait_timer = 0.0;
    gamesave_gather(*(int *)&delta_time);
    return;
  case 0xd:
    iVar1 = gamesave_load();
    if (iVar1 == 0) {
      if (float_1_0 < g_gamesave_wait_timer) {
        message_close();
        g_gamesave_format_step = 0xf;
        goto LAB_0040b376;
      }
LAB_0040b27c:
      gamesave_poll_status(*(int *)&delta_time);
      _Format = "Waiting for time to elapse\n";
    }
    else {
      _Format = "Waiting for load to complete\n";
    }
    goto LAB_0040b36e;
  case 0xe:
    iVar1 = gamesave_write();
    if (iVar1 == 0) {
      if (float_1_0 < g_gamesave_wait_timer) {
        message_close();
        g_gamesave_format_step = 0x10;
        goto LAB_0040b376;
      }
      goto LAB_0040b27c;
    }
    _Format = "Waiting for save to complete\n";
LAB_0040b36e:
    FID_conflict__wprintf(_Format);
LAB_0040b376:
    g_gamesave_wait_timer = g_gamesave_wait_timer + delta_time;
    return;
  case 0xf:
    message_show(0x1c1,0xff460000,0xb,0x2e,0,0);
    gamesave_apply();
    g_gamesave_format_step = 0x11;
    return;
  case 0x10:
    message_show(0x1bc,0xff460000,0xb,0x2e,0,0);
    nop_stub();
    g_gamesave_format_step = 0x11;
    g_gamesave_flag_c0c = 1;
    return;
  }
  return;
}


