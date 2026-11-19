#include "mfl_tier2.h"
/*0040aef0*/ extern int gamesave_request_delete();
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8be8*/ extern undefined4 g_gamesave_format_step;


/* gamesave_cb_boot_state */

/* "handler for the boot check state variable 0x8D8BE8" */

void gamesave_cb_boot_state(void)

{
  switch(g_gamesave_format_step) {
  case 6:
    g_gamesave_format_step = 7;
    return;
  case 7:
  case 8:
    gamesave_request_delete();
    return;
  case 0:
  case 0x11:
    g_gamesave_operation = 0;
    return;
  case 0xb:
    g_gamesave_format_step = 0xc;
    return;
  }
}
