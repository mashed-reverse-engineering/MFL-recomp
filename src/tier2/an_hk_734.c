#include "mfl_tier2.h"
/*0040aef0*/ extern int gamesave_request_delete();
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8bec*/ extern undefined4 g_gamesave_load_step;
/*008d8bf0*/ extern undefined4 g_gamesave_save_step;
/*008d8bf8*/ extern undefined4 g_gamesave_delete_step;
/* gamesave_cb_save_state */

/* "handler for the manual save state variable 0x8D8BF0" */

/*
 * Per-frame handler for the manual save-state step machine. Dispatches on
 * g_gamesave_save_step (0x8d8bf0) and moves it to the next step. No parameters,
 * no return value. (inferred: step meanings are not named in the dossier)
 */
void gamesave_cb_save_state(void)

{
  switch(g_gamesave_save_step) {
  /* step 5: finish, reset g_gamesave_operation to 0 (inferred: idle) and go to step 11 */
  case 5:
    goto switchD_0040bf2d_caseD_5;
  /* steps 6 and 7: call gamesave_request_delete each frame, step unchanged (inferred: waiting) */
  case 6:
  case 7:
    gamesave_request_delete();
    return;
  /* step 8: move to step 6 */
  case 8:
    g_gamesave_save_step = 6;
    return;
  /* steps 9 and 10: move back to step 3 */
  case 9:
  case 10:
    g_gamesave_save_step = 3;
    return;
  /* any other step: arm the load (step 6) and delete (step 8) sub-machines, then finish as step 5 */
  default:
    g_gamesave_load_step = 6;
    g_gamesave_delete_step = 8;
switchD_0040bf2d_caseD_5:
    g_gamesave_operation = 0;
    g_gamesave_save_step = 0xb;
    return;
  /* step 12: move to step 2 */
  case 0xc:
    g_gamesave_save_step = 2;
    return;
  }
}


