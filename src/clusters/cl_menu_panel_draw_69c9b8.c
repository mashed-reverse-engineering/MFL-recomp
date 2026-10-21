// cluster-entries: menu_panel_draw_69c9b8
/* Installed cluster menu_panel_draw_69c9b8 (tools/cluster_install.py from output/cluster_pilot_uc3/menu_panel_draw_69c9b8.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
extern int g_menu_panel_state_69c9b8;
extern int g_hud_69c9bc;
extern int g_menu_current;
extern int g_menu_selected_vehicle;
extern int g_menu_markers_enabled;
extern int g_menu_race_length_choice;

/* Draws the three rows of the race setup panel (vehicle, markers on/off, race length): per row an arrow at the left, the option text (text id from the
   current choice) and a second arrow behind the text, placed from the text width.  The selected row is drawn in black, the others in white, both with
   the panel alpha g_hud_69c9bc.  Nothing is drawn while the panel state is 0 and the alpha is below 0x60. */
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
void menu_panel_draw_69c9b8(void)
{
  char *item_list;
  float text_y, y, x, w, h;
  unsigned char colour_bytes[4];
  unsigned int colour;
  int row;
  int selected;
  int text_id;
  int text_w;
  int *arrow;

  if (g_menu_panel_state_69c9b8 == 0) {
    if (g_hud_69c9bc < 0x60) {
      return;
    }
    item_list = g_menu_records[g_menu_current].item_list;
  } else {
    item_list = g_menu_records[g_menu_current + 1].item_list;
  }
  text_y = (float)menu_item_y_position_r(menu_count_items_r(item_list), 0x1e);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  colour_bytes[3] = (unsigned char)g_hud_69c9bc;
  for (row = 0; row < 3; row++) {
    if (g_menu_panel_state_69c9b8 == 0) {
      selected = g_menu_records[g_menu_current].selected;
    } else {
      selected = g_menu_records[g_menu_current + 1].selected;
    }
    if (selected == row) {
      colour_bytes[0] = 0;
      colour_bytes[1] = 0;
      colour_bytes[2] = 0;
    } else {
      colour_bytes[0] = 0xff;
      colour_bytes[1] = 0xff;
      colour_bytes[2] = 0xff;
    }
    colour = *(unsigned int *)colour_bytes;
    x = (float)(int)screen_width() * 0.453125f;
    y = (text_y - 8.0f) * (float)(int)screen_height() * 0.0020833334f;
    w = (float)(int)screen_width() * 0.025f;
    h = (float)(int)screen_height() * 0.033333335f;
    draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, colour, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
    if (row == 0) {
      if (g_menu_selected_vehicle == 0) {
        text_id = 0xe6;
      } else if (g_menu_selected_vehicle == 1) {
        text_id = 0xd5;
      } else if (g_menu_selected_vehicle == 2) {
        text_id = 0xd6;
      } else if (g_menu_selected_vehicle == 3) {
        text_id = 0xd7;
      }
    } else if (row == 1) {
      text_id = (g_menu_markers_enabled != 0) + 0x5c;
    } else if (row == 2) {
      if (g_menu_race_length_choice == 0) {
        text_id = 0xe3;
      } else if (g_menu_race_length_choice == 1) {
        text_id = 0xe4;
      } else if (g_menu_race_length_choice == 2) {
        text_id = 0xe5;
      }
    }
    ui_rt2d_text_draw(text_id, 310.0f, text_y, colour, 0.7f, 0);
    text_w = (int)text_width(text_id, 0.7f);
    x = (float)(((int)screen_width() * (text_w + 0x13e)) / 0x280);
    arrow = find_texture_dict_c("Arrow");
    draw_sprite(arrow, x, y, w, h, colour, 1);
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
