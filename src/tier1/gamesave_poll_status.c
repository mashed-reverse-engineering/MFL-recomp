#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*004316f0*/ extern int message_close();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*0050b5e0*/ extern int nop_stub();
/*008d8be8*/ extern undefined4 g_gamesave_format_step;
/*008d8bec*/ extern undefined4 g_gamesave_load_step;
/*008d8bf0*/ extern undefined4 g_gamesave_save_step;
/*008d8bf4*/ extern undefined4 g_autosave_step;
/*008d8bf8*/ extern undefined4 g_gamesave_delete_step;


#define GAMESAVE_STEP_ERROR 2

/* gamesave_poll_status */

// Polls gamesave status stubs and resets all save/load/format state machines to error state on status change.
void gamesave_poll_status(void)
{
  int is_active;
  
  is_active = stub_return_zero();
  if (is_active == 0) {
    int has_changed;
    has_changed = stub_return_zero();
    if (has_changed != 0) {
      FID_conflict__wprintf("status change\n");
      message_close();
      /* Reset save, load, format, autosave, and delete state machines */
      g_gamesave_load_step = GAMESAVE_STEP_ERROR;
      g_gamesave_format_step = GAMESAVE_STEP_ERROR;
      g_gamesave_save_step = GAMESAVE_STEP_ERROR;
      g_autosave_step = GAMESAVE_STEP_ERROR;
      g_gamesave_delete_step = GAMESAVE_STEP_ERROR;
      nop_stub();
      return;
    }
  }
  return;
}
