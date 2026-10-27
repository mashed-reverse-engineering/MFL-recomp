#include "mfl_tier2.h"
/*0069c978*/ extern undefined4 g_menu_panel_state_69c978;
/*0069c980*/ extern undefined4 g_menu_panel_state_69c980;
/*0069c988*/ extern undefined4 g_menu_panel_state_69c988;
/*0069c990*/ extern undefined4 g_menu_panel_state_69c990;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069c9a0*/ extern undefined4 g_menu_panel_state_69c9a0;
/*0069c9a8*/ extern undefined4 g_menu_panel_state_69c9a8;
/*0069c9b0*/ extern undefined4 g_menu_panel_state_69c9b0;
/*0069c9b8*/ extern undefined4 g_menu_panel_state_69c9b8;
/*0069c9c0*/ extern undefined4 g_menu_panel_state_69c9c0;
/*0069c9c8*/ extern undefined4 g_menu_panel_state_69c9c8;
/*0069c9d0*/ extern undefined4 g_menu_panel_state_69c9d0;
/*0069c9d8*/ extern undefined4 g_menu_panel_state_69c9d8;
/*0069c9e0*/ extern undefined4 g_menu_panel_state_69c9e0;
/*0069c9e8*/ extern undefined4 g_menu_panel_state_69c9e8;
/*0069c9f0*/ extern undefined4 g_menu_panel_state_69c9f0;
/*0069c9f8*/ extern undefined4 g_menu_panel_state_69c9f8;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca08*/ extern undefined4 g_menu_panel_state_69ca08;
/*0069ca10*/ extern undefined4 g_menu_panel_state_69ca10;


/* menu_panel_states_decay */

/* "every panel state: 1 stays 1, otherwise becomes 0 or 2 (the 'was visible' state)" */

void menu_panel_states_decay(void)
{
  g_menu_panel_state_69c9a8 = (g_menu_panel_state_69c9a8 != 1) - 1 & 2;
  if (g_menu_panel_state_69c980 != 1) {
    g_menu_panel_state_69c980 = 0;
  }
  g_menu_panel_state_69c9c0 = (g_menu_panel_state_69c9c0 != 1) - 1 & 2;
  g_menu_panel_state_69c9a0 = (g_menu_panel_state_69c9a0 != 1) - 1 & 2;
  g_menu_panel_state_69c978 = (g_menu_panel_state_69c978 != 1) - 1 & 2;
  g_menu_panel_state_69c990 = (g_menu_panel_state_69c990 != 1) - 1 & 2;
  g_menu_panel_state_69c998 = (g_menu_panel_state_69c998 != 1) - 1 & 2;
  g_menu_panel_state_69c9c8 = (g_menu_panel_state_69c9c8 != 1) - 1 & 2;
  g_menu_panel_state_69ca08 = (g_menu_panel_state_69ca08 != 1) - 1 & 2;
  g_menu_panel_state_69ca10 = (g_menu_panel_state_69ca10 != 1) - 1 & 2;
  g_menu_panel_state_69c9f8 = (g_menu_panel_state_69c9f8 != 1) - 1 & 2;
  g_menu_panel_state_69ca00 = (g_menu_panel_state_69ca00 != 1) - 1 & 2;
  g_menu_panel_state_69c9f0 = (g_menu_panel_state_69c9f0 != 1) - 1 & 2;
  g_menu_panel_state_69c9b0 = (g_menu_panel_state_69c9b0 != 1) - 1 & 2;
  g_menu_panel_state_69c9b8 = (g_menu_panel_state_69c9b8 != 1) - 1 & 2;
  g_menu_panel_state_69c9d8 = (g_menu_panel_state_69c9d8 != 1) - 1 & 2;
  g_menu_panel_state_69c9e0 = (g_menu_panel_state_69c9e0 != 1) - 1 & 2;
  g_menu_panel_state_69c9e8 = (g_menu_panel_state_69c9e8 != 1) - 1 & 2;
  g_menu_panel_state_69c988 = (g_menu_panel_state_69c988 != 1) - 1 & 2;
  g_menu_panel_state_69c9d0 = (g_menu_panel_state_69c9d0 != 1) - 1 & 2;
  return;
}
