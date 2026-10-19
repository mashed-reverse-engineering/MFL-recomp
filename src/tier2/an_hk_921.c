#include "mfl_tier2.h"
/*0040aeb0*/ extern void gamesave_request_save_a(void);
/*0040bf20*/ extern void gamesave_cb_save_state(void);
/*0040bfb0*/ extern void gamesave_cb_load_state(void);
/*0040bfe0*/ extern void gamesave_cb_boot_state(void);
/*0040c050*/ extern void gamesave_cb_autosave_state(void);
/*004316f0*/ extern int message_close(void);
/*008d8be4*/ extern int g_gamesave_operation;
/*008d8bec*/ extern int g_gamesave_load_step;
/*008d8bf0*/ extern int g_gamesave_save_step;
/*008d8bf8*/ extern int g_gamesave_delete_step;
/* gamesave_cb_dispatch_a */

/* "calls the callback of the running save-game operation; an unknown operation resets the state" */

void gamesave_cb_dispatch_a(void)

{
  switch(g_gamesave_operation) {
  case 1:
    gamesave_cb_boot_state();
    return;
  case 2:
    gamesave_cb_load_state();
    return;
  case 3:
    gamesave_cb_save_state();
    return;
  case 4:
    gamesave_cb_autosave_state();
    return;
  case 6:
    message_close();
    gamesave_request_save_a();
    return;
  }
  g_gamesave_operation = 0;
  g_gamesave_delete_step = 8;
  g_gamesave_save_step = 0xb;
  g_gamesave_load_step = 6;
  return;
}
