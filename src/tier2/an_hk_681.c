#include "mfl_tier2.h"
/*0042fce0*/ extern int menu_fade_overlay_draw(void);
/*0069d1e4*/ extern undefined4 g_menu_fade_alpha;
/*008c8074*/ extern undefined4 g_menu_8c8074;
#define MENU_SCROLL_MAX (0x1ff)
#define MENU_FADE_ALPHA_MAX (0xff)
#define MENU_SCROLL_STEP (0x10)
/* menu_scroll_clamp */

/* "clamps g_menu_8c8074 to 0..0x1FF and applies it" */

/*
 * Menu scroll position clamp: keeps the scroll value g_menu_8c8074 in 0..0x1FF,
 * shows the menu fade overlay with the alpha clamped to 0..0xFF (the scroll value
 * is used as the alpha), then restores the previous g_menu_fade_alpha and moves
 * the scroll value back by 0x10 (inferred: one scroll step per call).
 * Takes no parameters and returns nothing. If the scroll value is below 1 it is
 * reset to 0 and the function returns early (no overlay draw, no step).
 */
void menu_scroll_clamp(void)

{
  undefined4 saved_fade_alpha;

  saved_fade_alpha = g_menu_fade_alpha;
  /* scroll value at or below 0: reset to top and stop */
  if ((int)g_menu_8c8074 < 1) {
    g_menu_8c8074 = 0;
    return;
  }
  /* clamp the scroll value to the upper limit MENU_SCROLL_MAX */
  if (MENU_SCROLL_MAX < (int)g_menu_8c8074) {
    g_menu_8c8074 = MENU_SCROLL_MAX;
  }
  g_menu_fade_alpha = g_menu_8c8074;
  /* clamp the fade alpha to MENU_FADE_ALPHA_MAX before drawing */
  if (MENU_FADE_ALPHA_MAX < (int)g_menu_8c8074) {
    g_menu_fade_alpha = MENU_FADE_ALPHA_MAX;
  }
  menu_fade_overlay_draw();
  /* put back the caller's fade alpha, then advance the scroll value by one step */
  g_menu_fade_alpha = saved_fade_alpha;
  g_menu_8c8074 = g_menu_8c8074 + -MENU_SCROLL_STEP;
  return;
}


