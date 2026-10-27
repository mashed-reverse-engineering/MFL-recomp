#include "mfl_tier2.h"
/*0040b050*/ extern void gamesave_message_show_pending(void);
/*0040b080*/ extern void gamesave_load_menu_state(float p0);
/*0040b410*/ extern void gamesave_autosave_state_machine(float p0);
/*0040b6b0*/ extern void gamesave_load_state_machine(float p0);
/*0040b920*/ extern void gamesave_format_state_machine(float p0);
/*0040bbd0*/ extern void LAB_0040bbd0(float p0);
/*008d8be4*/ extern int g_gamesave_operation;
/* gamesave_update */
/* "dispatches the running save-game operation to its state machine (delta time forwarded)" */
void gamesave_update(float delta_time)
{
  switch(g_gamesave_operation) {
  case 1:
    gamesave_load_menu_state(delta_time);
    return;
  case 2:
    gamesave_load_state_machine(delta_time);
    return;
  case 3:
    LAB_0040bbd0(delta_time);
    return;
  case 5:
    gamesave_format_state_machine(delta_time);
    return;
  case 4:
    gamesave_autosave_state_machine(delta_time);
    return;
  case 6:
    gamesave_message_show_pending();
    return;
  }
  return;
}
