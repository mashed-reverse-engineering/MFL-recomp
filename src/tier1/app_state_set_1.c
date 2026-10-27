#include "mfl_tier2.h"
/*0076e7f4*/ extern undefined4 g_app_state;


/* app_state_set_1 */

/* "g_app_state = 1" */

void app_state_set_1(void)
{
  g_app_state = APP_STATE_MAIN_LOOP_INIT;
  return;
}
