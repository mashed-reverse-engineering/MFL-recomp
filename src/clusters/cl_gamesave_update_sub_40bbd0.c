// cluster-entries: gamesave_update_sub_40bbd0
/* Installed cluster gamesave_update_sub_40bbd0 (tools/cluster_install.py from output/cluster_pilot_k/gamesave_update_sub_40bbd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00405d80*/ extern int gamesave_gather();
/*00405df0*/ extern int gamesave_write();
/*00405e20*/ extern int __cdecl gamesave_exists(undefined4 p0);
/*00405e40*/ extern int stub_return_zero();
/*0040af60*/ extern int gamesave_set_result_code();
/*0040b000*/ extern int gamesave_poll_status();
/*0040afa0*/ extern void gamesave_ui_save_done(int result_code);
/*004311b0*/ extern int message_show();
/*004316f0*/ extern int message_close();
/*004a34a0*/ extern int stub_return_one();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8bf0*/ extern undefined4 g_gamesave_save_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c08*/ extern float g_gamesave_wait_timer;
/*008d8c10*/ extern undefined4 g_gamesave_status_poll_count;
/*008d8c14*/ extern undefined4 g_gamesave_load_failed;
/* gamesave_update_sub_40bbd0 */

/* single caller gamesave_update */

void gamesave_update_sub_40bbd0(float param_1)

{
  int iVar1;
  undefined4 uVar2;
  char *_Format;
  
  switch(g_gamesave_save_step) {
  case 0:
  case 7:
  case 8:
  case 10:
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
      iVar1 = gamesave_exists(0);
      if (iVar1 == 0) {
        message_show(0x21b,0xff460000,2,0x2f,0x30,0);
        g_gamesave_save_step = 0xc;
        FID_conflict__wprintf("Waiting for save status\n");
        return;
      }
      g_gamesave_save_step = 2;
    }
    FID_conflict__wprintf("Waiting for save status\n");
    return;
  case 2:
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_save_done(1);
      return;
    }
    iVar1 = stub_return_one(0);
    if (iVar1 == 0) {
      gamesave_ui_save_done(3);
      return;
    }
    iVar1 = gamesave_exists(0);
    if (iVar1 != 0) {
      g_gamesave_save_step = 9;
      return;
    }
    iVar1 = stub_return_one(0);
    if (iVar1 != 0) {
      g_gamesave_save_step = 3;
      return;
    }
    gamesave_ui_save_done(2);
    return;
  case 3:
    g_gamesave_load_failed = 0;
    message_show(0x238,0xff460000,0,0,0,0);
    gamesave_gather();
    g_gamesave_wait_timer = 0.0;
    g_gamesave_save_step = 4;
    gamesave_set_result_code(0x6);
    return;
  case 4:
    iVar1 = gamesave_write();
    if (iVar1 == 0) {
      if (float_1_0 < g_gamesave_wait_timer) {
        message_close();
        g_gamesave_save_step = 5;
        goto LAB_0040be88;
      }
      gamesave_poll_status();
      _Format = "Waiting for time to elapse\n";
    }
    else {
      _Format = "Waiting for save to complete\n";
    }
    FID_conflict__wprintf(_Format);
LAB_0040be88:
    g_gamesave_wait_timer = g_gamesave_wait_timer + param_1;
    return;
  case 5:
    switch(g_gamesave_result_code) {
    case 0:
      message_show(0x1bc,0xff460000,0xb,0x2e,0,0);
      nop_stub();
      g_gamesave_save_step = 0xb;
      return;
    case 1:
      message_show(0x1bf,0xff460000,0xb,0x2e,0,0);
      g_gamesave_save_step = 0;
      return;
    case 2:
      message_show(0x1ba,0xff460000,0xb,0x2e,0,0);
      g_gamesave_save_step = 0;
      return;
    case 3:
      message_show(0x24e,0xff460000,2,0x2f,0x30,0);
      g_gamesave_save_step = 8;
      return;
    case 6:
      message_show(0x1bb,0xff460000,0xb,0x2e,0,0);
      g_gamesave_load_failed = 1;
      g_gamesave_result_code = 7;
      return;
    }
    break;
  case 6:
    message_show(0x1c5,0xff460000,2,0x2f,0x30,0);
    g_gamesave_save_step = 7;
    return;
  case 9:
    if (g_gamesave_load_failed == 0) {
      uVar2 = 0x1cf;
    }
    else {
      uVar2 = 0x24d;
    }
    message_show(uVar2,0xff460000,2,0x2f,0x30,0);
    g_gamesave_save_step = 10;
    return;
  case 0xb:
    g_gamesave_operation = 0;
  }
  return;
}

