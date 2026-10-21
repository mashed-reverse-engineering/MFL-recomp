// cluster-entries: hud_arrow_draw
/* Installed cluster hud_arrow_draw (tools/cluster_install.py from output/cluster_pilot_uc3/hud_arrow_draw.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* compile: /GS */
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0040d0f0*/ extern int *find_texture_dict_c(const char *name);
/*0042c440*/ extern int ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*0042c8e0*/ extern float text_width(int text_id, float scale);
/*00483130*/ extern void draw_sprite(int *texture, float x, float y, float w, float h, unsigned int colour, int mode);
/*004832d0*/ extern void draw_sprite_uv(int *texture, float x, float y, float w, float h, unsigned int colour,
                                        float u0, float u1, float v0, float v1, int scale_mode, int linear);
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
extern int g_menu_panel_state_69c980;
extern int g_menu_69c984;
extern int g_menu_69ca1c;
extern int g_menu_current;
extern unsigned int g_menu_player_count_index;
extern int g_menu_60ad5c;
extern int g_menu_60ad60;

/* Draws the two selection arrows (a mirrored one at the left of the entry, one at the right behind its text, the right one placed from the text
   width) next to the selected entry of the three rows of a small menu and the three rows' texts (text ids per row: player count / two toggles).
   Fades with the panel alpha g_menu_69c984; nothing is drawn while the panel is closed and the alpha is below 0x60. */
void hud_arrow_draw(void)
{
  float x, y, w, h;
  float text_y;
  int text_id;
  int selected;
  int row;
  unsigned char colours[2][4];   /* RGBA bytes: [0] dim, [1] bright (the byte array is why the original is a /GS function) */
  int *arrow;
  int text_w;

  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  if (g_menu_panel_state_69c980 == 0) {
    selected = g_menu_records[g_menu_current].selected;
  } else {
    int page = g_menu_current;
    if (g_menu_69ca1c == 0) {
      page = g_menu_current - 2;
    }
    selected = g_menu_records[page + 1].selected;
  }
  colours[0][0] = 0xff;
  colours[0][1] = 0xff;
  colours[0][2] = 0xff;
  colours[0][3] = (unsigned char)g_menu_69c984;
  if (g_menu_panel_state_69c980 == 0 && g_menu_69c984 < 0x60) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
    return;
  }
  colours[1][0] = 0;
  colours[1][1] = 0;
  colours[1][2] = 0;
  colours[1][3] = (unsigned char)g_menu_69c984;
  x = (float)(int)screen_width() * 0.40625f;
  y = (float)(int)screen_height() * 0.42291668f;
  w = (float)(int)screen_width() * 0.025f;
  h = (float)(int)screen_height() * 0.033333335f;
  text_y = 211.0f;
  text_id = 0x43530000;
  for (row = 0; row < 3; row++) {
    if (row == selected) {
      draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, *(unsigned int *)colours[1], 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
    }
    if (row == 0) {
      switch (g_menu_player_count_index) {
      case 0:
        text_id = 0x3b;
        break;
      case 1:
        text_id = 0x159;
        break;
      case 2:
        text_id = 0x62;
        break;
      case 3:
        text_id = 99;
      }
    } else if (row == 1) {
      text_id = (g_menu_60ad5c == 1) * 8 + 100;
    } else if (row == 2) {
      text_id = (g_menu_60ad60 == 1) * 8 + 0x65;
    }
    ui_rt2d_text_draw(text_id, 280.0f, text_y, (row == selected) ? *(unsigned int *)colours[1] : *(unsigned int *)colours[0], 0.7f, 0);
    if (row == selected) {
      text_w = (int)text_width(text_id, 0.7f);
      x = (float)(((int)screen_width() * (text_w + 0x120)) / 0x280);
      arrow = find_texture_dict_c("Arrow");
      draw_sprite(arrow, x, y, w, h, *(unsigned int *)colours[1], 1);
    }
    y = (float)(((int)screen_height() * 0x1e) / 0x1e0) + y;
    text_y = text_y + 30.0f;
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}

