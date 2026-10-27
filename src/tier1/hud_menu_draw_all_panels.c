#include "mfl_tier2.h"
/*004345d0*/ extern int hud_arrow_draw();
/*00435100*/ extern int menu_panel_draw_69c9b8();
/*00435420*/ extern int menu_panel_draw_69c9e0();
/*004356b0*/ extern int menu_panel_draw_69c9e8();
/*00436550*/ extern int options_menu_draw();
/*00436c00*/ extern int menu_panel_draw_69ca08();
/*00436e70*/ extern int menu_panel_draw_69ca10();
/*004370e0*/ extern int menu_panel_draw_69ca00();
/*00437340*/ extern int menu_panel_draw_69c9f0();
/*004393f0*/ extern int menu_draw_controller_select();
/*0043abd0*/ extern int results_screen_draw();
/*004408c0*/ extern int menu_draw_event_descriptions();
/*00441c80*/ extern int menu_draw_skill_level_select();
/*004420a0*/ extern int menu_draw_team_select();
/*00442580*/ extern int menu_draw_event_icons();
/*0069c978*/ extern undefined4 g_menu_panel_state_69c978;
/*0069c980*/ extern undefined4 g_menu_panel_state_69c980;
/*0069c988*/ extern undefined4 g_menu_panel_state_69c988;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069c9a8*/ extern undefined4 g_menu_panel_state_69c9a8;
/*0069c9b0*/ extern undefined4 g_menu_panel_state_69c9b0;
/*0069c9b8*/ extern undefined4 g_menu_panel_state_69c9b8;
/*0069c9c0*/ extern undefined4 g_menu_panel_state_69c9c0;
/*0069c9c8*/ extern undefined4 g_menu_panel_state_69c9c8;
/*0069c9e0*/ extern undefined4 g_menu_panel_state_69c9e0;
/*0069c9e8*/ extern undefined4 g_menu_panel_state_69c9e8;
/*0069c9f0*/ extern undefined4 g_menu_panel_state_69c9f0;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca08*/ extern undefined4 g_menu_panel_state_69ca08;
/*0069ca10*/ extern undefined4 g_menu_panel_state_69ca10;


/* hud_menu_draw_all_panels */

/* "draws the menu panels that are visible" */

void hud_menu_draw_all_panels(void)
{
  if (g_menu_panel_state_69c978 != 0) {
    menu_draw_controller_select();
  }
  if (g_menu_panel_state_69c9a8 != 0) {
    menu_draw_skill_level_select();
  }
  if (g_menu_panel_state_69c980 != 0) {
    hud_arrow_draw();
  }
  if (g_menu_panel_state_69c9c0 != 0) {
    menu_draw_event_icons(0);
  }
  if (g_menu_panel_state_69c998 != 0) {
    results_screen_draw();
  }
  if (g_menu_panel_state_69c9c8 != 0) {
    options_menu_draw();
  }
  if (g_menu_panel_state_69ca08 != 0) {
    menu_panel_draw_69ca08();
  }
  if (g_menu_panel_state_69ca10 != 0) {
    menu_panel_draw_69ca10();
  }
  if (g_menu_panel_state_69ca00 != 0) {
    menu_panel_draw_69ca00();
  }
  if (g_menu_panel_state_69c9f0 != 0) {
    menu_panel_draw_69c9f0();
  }
  if (g_menu_panel_state_69c9b0 != 0) {
    menu_draw_team_select();
  }
  if (g_menu_panel_state_69c9b8 != 0) {
    menu_panel_draw_69c9b8();
  }
  if (g_menu_panel_state_69c9e0 != 0) {
    menu_panel_draw_69c9e0();
  }
  if (g_menu_panel_state_69c9e8 != 0) {
    menu_panel_draw_69c9e8();
  }
  if (g_menu_panel_state_69c988 != 0) {
    menu_draw_event_descriptions(0);
  }
  return;
}
