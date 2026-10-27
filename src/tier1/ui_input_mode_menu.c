#include "mfl_tier2.h"
/*007f01f4*/ extern undefined4 g_input_mode;


/* ui_input_mode_menu */

/* "g_ui_7f01f4 = 0x1000: input goes to the menu" */

void ui_input_mode_menu(void)
{
  g_input_mode = 0x1000;
  return;
}
