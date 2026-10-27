/* compile: /GS */
#include "mfl_tier2.h"
/*00403890*/ extern void hud_flash_text(void);
/*0042cdd0*/ extern void ui_draw_texture_640_default(void *texture, float x, float y, float w, float h, int mode);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0076e7f0*/ extern void *g_hud_76e7f0;

/* Draws the HUD overlay texture (if loaded) and the flashing text on top of it with the z test and z writes off. */
void hud_flash_text_overlay(void)
{
  char text[16];      /* unused in the release build */
  if (g_hud_76e7f0 != 0) {
    ui_draw_texture_640_default(g_hud_76e7f0, 320.0f, 200.0f, 480.0f, 240.0f, 2);
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  hud_flash_text();
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}
