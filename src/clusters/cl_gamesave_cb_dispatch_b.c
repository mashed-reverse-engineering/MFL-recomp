// cluster-entries: gamesave_cb_dispatch_b
/* Installed cluster gamesave_cb_dispatch_b (tools/cluster_install.py from output/cluster_pilot_pg2/gamesave_cb_dispatch_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004316f0*/ extern void message_close(void);
/*008d8be4*/ extern int g_gamesave_operation;
/*008d8be8*/ extern int g_gamesave_format_step;
/*008d8bec*/ extern int g_gamesave_load_step;
/*008d8bf0*/ extern int g_gamesave_save_step;
/*008d8bf4*/ extern int g_autosave_step;
/*008d8bf8*/ extern int g_gamesave_delete_step;

/* g_gamesave_operation: the save operation in progress, named by the step variable each one polls */
enum GamesaveOperation {
  GAMESAVE_OP_NONE = 0,
  GAMESAVE_OP_FORMAT = 1,
  GAMESAVE_OP_LOAD = 2,
  GAMESAVE_OP_SAVE = 3,
  GAMESAVE_OP_AUTOSAVE = 4,
  GAMESAVE_OP_5 = 5,                  /* nothing to do */
  GAMESAVE_OP_MESSAGE = 6
};

/* step values of the state machines that end or abort an operation */
#define FORMAT_STEP_DONE_FIRST 6      /* 6..8 and 11 end the format operation */
#define FORMAT_STEP_DONE_LAST 8
#define FORMAT_STEP_FAILED 0xb
#define LOAD_STEP_FINISHED 7
#define LOAD_STEP_ABORTED 6
#define SAVE_STEP_WAIT_LOAD 0xc       /* the save waits for the load: restart it */
#define SAVE_STEP_ABORTED 0xb
#define DELETE_STEP_ABORTED 8
#define AUTOSAVE_STEP_FIRST 6         /* 6..10: the autosave message may be closed */
#define AUTOSAVE_STEP_LAST 10

/* gamesave_cb_dispatch_b: cancels the running save operation (called with the operation's abort / close request): depending on
   the operation it ends it when its state machine is in a state that allows it, or forces the other state machines into their
   aborted states */
void gamesave_cb_dispatch_b(void)
{
  int old_load_step;

  switch (g_gamesave_operation) {
  case GAMESAVE_OP_FORMAT:
    if (g_gamesave_format_step >= FORMAT_STEP_DONE_FIRST &&
        (g_gamesave_format_step <= FORMAT_STEP_DONE_LAST || g_gamesave_format_step == FORMAT_STEP_FAILED)) {
      g_gamesave_operation = GAMESAVE_OP_NONE;
    }
    break;
  case GAMESAVE_OP_LOAD:
    old_load_step = g_gamesave_load_step;
    g_gamesave_operation = GAMESAVE_OP_NONE;
    g_gamesave_load_step = LOAD_STEP_ABORTED;
    if (old_load_step != LOAD_STEP_FINISHED) {
      g_gamesave_delete_step = DELETE_STEP_ABORTED;
      g_gamesave_save_step = SAVE_STEP_ABORTED;
    }
    break;
  case GAMESAVE_OP_SAVE:
    if (g_gamesave_save_step == SAVE_STEP_WAIT_LOAD) {
      g_gamesave_load_step = 0;
    } else {
      g_gamesave_operation = GAMESAVE_OP_NONE;
      g_gamesave_delete_step = DELETE_STEP_ABORTED;
      g_gamesave_save_step = SAVE_STEP_ABORTED;
    }
    break;
  case GAMESAVE_OP_AUTOSAVE:
    if (g_autosave_step >= AUTOSAVE_STEP_FIRST && g_autosave_step <= AUTOSAVE_STEP_LAST) {
      message_close();
      g_gamesave_operation = GAMESAVE_OP_NONE;
    }
    break;
  case GAMESAVE_OP_MESSAGE:
    message_close();
    g_gamesave_operation = GAMESAVE_OP_NONE;
    break;
  default:
    break;
  }
}

