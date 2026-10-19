// cluster-entries: menu_fade_overlay_draw
/* Installed cluster menu_fade_overlay_draw (tools/cluster_install.py from output/cluster_pilot_zy/menu_fade_overlay_draw.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0069d168*/ extern RwIm2DVertex g_course_69d168[4];
/*0069d1e4*/ extern unsigned int g_menu_fade_alpha;
/*0069cc2c*/ extern short g_ui_69cc2c;
/*0069cc2e*/ extern short g_ui_69cc2e;

/* Draws the full-screen fade overlay: a black quad (screen size taken from g_ui_69cc2c/2e) whose alpha is the low byte of g_menu_fade_alpha; nothing when it is 0. `unused` is the dummy argument the callers push (never read). */
void menu_fade_overlay_draw(int unused)
{
  RwIm2DVertex *v = g_course_69d168;
  int i;

  RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)0x1);
  v[0].x = 0.0f;
  v[0].y = 0.0f;
  v[1].x = (float)(int)g_ui_69cc2c;
  v[1].y = 0.0f;
  v[2].x = 0.0f;
  v[2].y = (float)(int)g_ui_69cc2e;
  v[3].x = v[1].x;
  v[3].y = v[2].y;
  for (i = 0; i < 4; i++) {
    v[i].emissiveColor = 0xd0808080;
    v[i].z = RwIm2DGetNearScreenZ();
    v[i].rhw = 1.0f;
  }
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0x1);
  RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)0x5);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)0x6);
  v[0].emissiveColor = g_menu_fade_alpha << 0x18;
  v[1].emissiveColor = g_menu_fade_alpha << 0x18;
  v[2].emissiveColor = g_menu_fade_alpha << 0x18;
  v[3].emissiveColor = g_menu_fade_alpha << 0x18;
  if (g_menu_fade_alpha != 0) {
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, v, 4);
  }
}

