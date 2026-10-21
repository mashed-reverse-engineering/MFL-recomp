// cluster-entries: menu_panel_fades_update
/* Installed cluster menu_panel_fades_update (tools/cluster_install.py from output/cluster_pilot_uc3/menu_panel_fades_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern int g_menu_panel_state_69c9a8;
extern int g_menu_panel_state_69c980;
extern int g_menu_panel_state_69c9c0;
extern int g_menu_panel_state_69c9a0;
extern int g_menu_panel_state_69c978;
extern int g_menu_panel_state_69c990;
extern int g_menu_panel_state_69c998;
extern int g_menu_panel_state_69c9c8;
extern int g_menu_panel_state_69ca08;
extern int g_menu_panel_state_69ca10;
extern int g_menu_panel_state_69c9f8;
extern int g_menu_panel_state_69ca00;
extern int g_menu_panel_state_69c9f0;
extern int g_menu_panel_state_69c9b0;
extern int g_menu_panel_state_69c9b8;
extern int g_menu_panel_state_69c9d8;
extern int g_menu_panel_state_69c9e0;
extern int g_menu_panel_state_69c9e8;
extern int g_menu_panel_state_69c988;
extern int g_menu_panel_state_69c9d0;
extern float g_menu_select_flash;
extern unsigned char g_menu_69c97c;
extern unsigned char g_menu_69c9ac;
extern unsigned char g_menu_69c984;
extern unsigned char g_menu_69c9c4;
extern unsigned char g_menu_update_69c9a4;
extern unsigned char g_menu_update_69c994;
extern unsigned char g_frontend_69c99c;
extern unsigned char g_options_69c9cc;
extern unsigned char g_menu_69ca0c;
extern unsigned char g_menu_69ca14;
extern unsigned char g_menu_update_69c9fc;
extern unsigned char g_menu_69ca04;
extern unsigned char g_hud_69c9f4;
extern unsigned char g_menu_69c9b4;
extern unsigned char g_hud_69c9bc;
extern unsigned char g_menu_update_69c9dc;
extern unsigned char g_hud_69c9e4;
extern unsigned char g_hud_69c9ec;
extern unsigned char g_menu_69c98c;
extern void menu_controller_input_update(void);
extern void menu_player_direction_navigate(void);
extern void tutorial_text_update(void);
extern void menu_controller_navigation_update(void);

/* Steps the fade of the 20 frontend panels once per frame.  g_menu_select_flash (0..1) is the fade phase: while it is 0 every panel state other than 1 (= fading in)
   is reset to 0.  A panel fading in (state 1) gets the alpha byte (1 - flash) * 255, a panel fading out (state 2) flash * 255, any other state 0; four panels also
   run their per frame update while fading in.  (The first alpha of panel 0xc9a8 clears the low byte of the state instead of the alpha: a quirk of the original.) */
void menu_panel_fades_update(void)
{
  if (g_menu_select_flash == 0.0f) {
    if (g_menu_panel_state_69c9a8 != 1) {
      g_menu_panel_state_69c9a8 = 0;
    }
    if (g_menu_panel_state_69c980 != 1) {
      g_menu_panel_state_69c980 = 0;
    }
    if (g_menu_panel_state_69c9c0 != 1) {
      g_menu_panel_state_69c9c0 = 0;
    }
    if (g_menu_panel_state_69c9a0 != 1) {
      g_menu_panel_state_69c9a0 = 0;
    }
    if (g_menu_panel_state_69c978 != 1) {
      g_menu_panel_state_69c978 = 0;
    }
    if (g_menu_panel_state_69c990 != 1) {
      g_menu_panel_state_69c990 = 0;
    }
    if (g_menu_panel_state_69c998 != 1) {
      g_menu_panel_state_69c998 = 0;
    }
    if (g_menu_panel_state_69c9c8 != 1) {
      g_menu_panel_state_69c9c8 = 0;
    }
    if (g_menu_panel_state_69ca08 != 1) {
      g_menu_panel_state_69ca08 = 0;
    }
    if (g_menu_panel_state_69ca10 != 1) {
      g_menu_panel_state_69ca10 = 0;
    }
    if (g_menu_panel_state_69c9f8 != 1) {
      g_menu_panel_state_69c9f8 = 0;
    }
    if (g_menu_panel_state_69ca00 != 1) {
      g_menu_panel_state_69ca00 = 0;
    }
    if (g_menu_panel_state_69c9f0 != 1) {
      g_menu_panel_state_69c9f0 = 0;
    }
    if (g_menu_panel_state_69c9b0 != 1) {
      g_menu_panel_state_69c9b0 = 0;
    }
    if (g_menu_panel_state_69c9b8 != 1) {
      g_menu_panel_state_69c9b8 = 0;
    }
    if (g_menu_panel_state_69c9d8 != 1) {
      g_menu_panel_state_69c9d8 = 0;
    }
    if (g_menu_panel_state_69c9e0 != 1) {
      g_menu_panel_state_69c9e0 = 0;
    }
    if (g_menu_panel_state_69c9e8 != 1) {
      g_menu_panel_state_69c9e8 = 0;
    }
    if (g_menu_panel_state_69c988 != 1) {
      g_menu_panel_state_69c988 = 0;
    }
    if (g_menu_panel_state_69c9d0 != 1) {
      g_menu_panel_state_69c9d0 = 0;
    }
  }
  if (g_menu_panel_state_69c978 == 1) {
    g_menu_69c97c = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
    menu_controller_input_update();
  } else if (g_menu_panel_state_69c978 == 2) {
    g_menu_69c97c = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    *(unsigned char *)&g_menu_69c97c = 0;
  }
  if (g_menu_panel_state_69c9a8 == 1) {
    g_menu_69c9ac = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
    menu_player_direction_navigate();
  } else if (g_menu_panel_state_69c9a8 == 2) {
    g_menu_69c9ac = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    *(unsigned char *)&g_menu_panel_state_69c9a8 = 0;
  }
  if (g_menu_panel_state_69c980 == 1) {
    g_menu_69c984 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c980 == 2) {
    g_menu_69c984 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_69c984 = 0;
  }
  if (g_menu_panel_state_69c9c0 == 1) {
    g_menu_69c9c4 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9c0 == 2) {
    g_menu_69c9c4 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_69c9c4 = 0;
  }
  if (g_menu_panel_state_69c9a0 == 1) {
    g_menu_update_69c9a4 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9a0 == 2) {
    g_menu_update_69c9a4 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_update_69c9a4 = 0;
  }
  if (g_menu_panel_state_69c990 == 1) {
    g_menu_update_69c994 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c990 == 2) {
    g_menu_update_69c994 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_update_69c994 = 0;
  }
  if (g_menu_panel_state_69c998 == 1) {
    g_frontend_69c99c = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
    tutorial_text_update();
  } else if (g_menu_panel_state_69c998 == 2) {
    g_frontend_69c99c = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    *(unsigned char *)&g_frontend_69c99c = 0;
  }
  if (g_menu_panel_state_69c9c8 == 1) {
    g_options_69c9cc = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9c8 == 2) {
    g_options_69c9cc = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_options_69c9cc = 0;
  }
  if (g_menu_panel_state_69ca08 == 1) {
    g_menu_69ca0c = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69ca08 == 2) {
    g_menu_69ca0c = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_69ca0c = 0;
  }
  if (g_menu_panel_state_69ca10 == 1) {
    g_menu_69ca14 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69ca10 == 2) {
    g_menu_69ca14 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_69ca14 = 0;
  }
  if (g_menu_panel_state_69c9f8 == 1) {
    g_menu_update_69c9fc = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9f8 == 2) {
    g_menu_update_69c9fc = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_update_69c9fc = 0;
  }
  if (g_menu_panel_state_69ca00 == 1) {
    g_menu_69ca04 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69ca00 == 2) {
    g_menu_69ca04 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_69ca04 = 0;
  }
  if (g_menu_panel_state_69c9f0 == 1) {
    g_hud_69c9f4 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9f0 == 2) {
    g_hud_69c9f4 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_hud_69c9f4 = 0;
  }
  if (g_menu_panel_state_69c9b0 == 1) {
    g_menu_69c9b4 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
    menu_controller_navigation_update();
  } else if (g_menu_panel_state_69c9b0 == 2) {
    g_menu_69c9b4 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    *(unsigned char *)&g_menu_69c9b4 = 0;
  }
  if (g_menu_panel_state_69c9b8 == 1) {
    g_hud_69c9bc = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9b8 == 2) {
    g_hud_69c9bc = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_hud_69c9bc = 0;
  }
  if (g_menu_panel_state_69c9d8 == 1) {
    g_menu_update_69c9dc = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9d8 == 2) {
    g_menu_update_69c9dc = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_menu_update_69c9dc = 0;
  }
  if (g_menu_panel_state_69c9e0 == 1) {
    g_hud_69c9e4 = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9e0 == 2) {
    g_hud_69c9e4 = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_hud_69c9e4 = 0;
  }
  if (g_menu_panel_state_69c9e8 == 1) {
    g_hud_69c9ec = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c9e8 == 2) {
    g_hud_69c9ec = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    g_hud_69c9ec = 0;
  }
  if (g_menu_panel_state_69c988 == 1) {
    g_menu_69c98c = (unsigned char)((1.0f - g_menu_select_flash) * 255.0f);
  } else if (g_menu_panel_state_69c988 == 2) {
    g_menu_69c98c = (unsigned char)(g_menu_select_flash * 255.0f);
  } else {
    *(unsigned char *)&g_menu_69c98c = 0;
  }
}

