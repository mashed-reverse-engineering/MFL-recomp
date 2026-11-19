#include "mfl_tier2.h"
/*0040aef0*/ extern int gamesave_request_delete();
/*0050b5e0*/ extern int nop_stub();
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8bf4*/ extern undefined4 g_autosave_step;


/* gamesave_cb_autosave_state */

/* "handler for the autosave state variable 0x8D8BF4" */

void gamesave_cb_autosave_state(void)

{
  switch(g_autosave_step) {
  case 9:
  case 10:
    g_autosave_step = 1;
    nop_stub();
    return;
  case 8:
    g_autosave_step = 6;
    return;
  case 6:
  case 7:
    gamesave_request_delete();
    return;
  case 0:
    g_gamesave_operation = 0;
    break;
  }
  return;
}
