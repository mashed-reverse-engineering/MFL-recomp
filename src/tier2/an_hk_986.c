#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0069c9e8*/ extern undefined4 g_menu_panel_state_69c9e8;
/*0069c9ec*/ extern int g_hud_69c9ec;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7d0*/ extern undefined4 g_menu_7ef7d0;
/* menu_panel_draw_69c9e8 */

/* "panel with state flag g_menu_panel_state_69c9e8 (fade counter 69c9ec)" */

void menu_panel_draw_69c9e8(void)

{
  int iStack_4;
  
  if ((g_menu_panel_state_69c9e8 != 2) &&
     ((g_menu_panel_state_69c9e8 != 0 || (0x5f < g_hud_69c9ec)))) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    iStack_4 = g_hud_69c9ec << 0x18;
    if (g_menu_records[g_menu_current].selected == 1) {
      ui_rt2d_text_draw(0x24a,0x43a00000,0x43a50000,iStack_4,0x3f19999a,2);
      ui_rt2d_text_draw(0x249,0x43a00000,0x43af0000,iStack_4,0x3f19999a,2);
      ui_rt2d_text_draw(0x282,0x43a00000,0x43b90000,iStack_4,0x3f19999a,2);
      ui_rt2d_text_draw(0x283,0x43a00000,0x43c30000,iStack_4,0x3f19999a,2);
    }
    else {
      ui_rt2d_text_draw(0x24c,0x43a00000,0x43a50000,iStack_4,0x3f19999a,2);
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    g_menu_7ef7d0 = 0;
  }
  return;
}


