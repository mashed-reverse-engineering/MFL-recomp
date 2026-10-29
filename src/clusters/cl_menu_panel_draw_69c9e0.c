// cluster-entries: menu_panel_draw_69c9e0
/* Installed cluster menu_panel_draw_69c9e0 (tools/cluster_install.py from output/cluster_pilot_uc3/menu_panel_draw_69c9e0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
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
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
extern int g_menu_panel_state_69c9e0;
extern int g_hud_69c9e4;
extern int g_menu_current;
extern int g_menu_8c8080[4];

/* Draws the controller (rumble) rows of the options panel: for each of the 4 rows that is in use (g_menu_item_anims[row + 1].ref_a, the idle text id, is not 0xc6) a left arrow, all in black, the text "rumble
   off / on" (text id 0x5c + enabled) of the row's player and a second arrow behind the text (placed from the text width).  Rows of disabled entries
   are drawn at a quarter of the alpha.  Nothing is drawn while the panel state is 2, or 0 with the alpha below 0x60. */
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
void menu_panel_draw_69c9e0(void)
{
  char *item_list;
  float text_y, y, x, w, h;
  unsigned char alpha;
  unsigned char row_alpha[4];
  unsigned int colour;
  int row;
  int text_id;
  int text_w;
  int *arrow;

  if (g_menu_panel_state_69c9e0 == 2) {
    return;
  }
  if (g_menu_panel_state_69c9e0 == 0 && g_hud_69c9e4 < 0x60) {
    return;
  }
  if (g_menu_panel_state_69c9e0 == 1) {
    item_list = g_menu_records[g_menu_current].item_list;
  } else {
    item_list = g_menu_records[g_menu_current + 1].item_list;
  }
  text_y = (float)menu_item_y_position_r(menu_count_items_r(item_list), 0x1e);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  alpha = (unsigned char)g_hud_69c9e4;
  for (row = 0; row < 4; row++) {
    row_alpha[3] = alpha;
    if (g_menu_records[g_menu_current].entry_enabled[row] == 0) {
      row_alpha[3] = alpha >> 2;
    }
    if (g_menu_item_anims[row + 1].ref_a != 0xc6) {
      row_alpha[0] = 0;
      row_alpha[1] = 0;
      row_alpha[2] = 0;
      colour = *(unsigned int *)row_alpha;
      x = (float)(int)screen_width() * 0.515625f;
      y = (text_y - 8.0f) * (float)(int)screen_height() * 0.0020833334f;
      w = (float)(int)screen_width() * 0.025f;
      h = (float)(int)screen_height() * 0.033333335f;
      draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, colour, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
      text_id = (g_player_inputs[g_menu_8c8080[row]].rumble_enabled != 0) + 0x5c;
      ui_rt2d_text_draw(text_id, 350.0f, text_y, colour, 0.7f, 0);
      text_w = (int)text_width(text_id, 0.7f);
      x = (float)(((int)screen_width() * (text_w + 0x166)) / 0x280);
      arrow = find_texture_dict_c("Arrow");
      draw_sprite(arrow, x, y, w, h, colour, 1);
      alpha = (unsigned char)g_hud_69c9e4;
    }
    text_y = text_y + 30.0f;
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
}

/* menu_count_items_r: C -> original menu_count_items (registers edx) */
__declspec(naked) int menu_count_items_r(char * p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call menu_count_items
        ret
    }
}

/* menu_item_y_position_r: C -> original menu_item_y_position (registers eax,ecx) */
__declspec(naked) int menu_item_y_position_r(int p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}
