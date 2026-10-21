// cluster-entries: menu_panel_draw_69c9f0
/* Installed cluster menu_panel_draw_69c9f0 (tools/cluster_install.py from output/cluster_pilot_uc3/menu_panel_draw_69c9f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*0040d0f0*/ extern int *find_texture_dict_c(const char *name);
/*0042c8e0*/ extern float text_width(int text_id, float scale);
/*00483130*/ extern void draw_sprite(int *texture, float x, float y, float w, float h, unsigned int colour, int mode);
/*004832d0*/ extern void draw_sprite_uv(int *texture, float x, float y, float w, float h, unsigned int colour,
                                        float u0, float u1, float v0, float v1, int scale_mode, int linear);
/*004824a0*/ extern int draw_rect(float x, float y, float w, float h, unsigned int colour);
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
extern int g_menu_panel_state_69c9f0;
extern int g_hud_69c9f4;
extern int g_menu_current;
extern float g_menu_69cfc4;

/* Draws the arrows of an options page and, next to the first row, a slider bar: a frame (106 x 16 with 3 pixel borders) filled with the value
   g_menu_69cfc4 (0..1, bar width 100 * value) in orange.  Each row gets an arrow at the left and one behind the row (placed from a fixed width of
   102, rows 2 and 3 from the text width of text id = item count [sic, as in the original]).  Rows have the panel alpha g_hud_69c9f4; nothing is drawn
   while the panel state is 0 and the alpha is below 0x60. */
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
void menu_panel_draw_69c9f0(void)
{
  unsigned char orange[4];     /* RGBA of the slider fill, alpha follows the panel */
  unsigned char black[4];      /* RGBA black, panel alpha */
  unsigned char half[4];       /* RGBA black, half the panel alpha */
  char *item_list;
  float text_y, y, y2, x, w, h, fill;
  int count;
  int row;
  int alpha;
  int width;
  int *arrow;

  orange[0] = 0xd8;
  orange[1] = 0x80;
  orange[2] = 0x20;
  orange[3] = 0xff;
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  alpha = g_hud_69c9f4;
  if (g_menu_panel_state_69c9f0 == 0) {
    if (g_hud_69c9f4 < 0x60) {
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
      return;
    }
  } else if (g_menu_panel_state_69c9f0 == 1) {
    item_list = g_menu_records[g_menu_current].item_list;
    goto have_list;
  }
  item_list = g_menu_records[g_menu_current + 1].item_list;
have_list:
  count = menu_count_items_r(item_list);
  text_y = (float)menu_item_y_position_r(count, 0x1e);
  if (count > 0) {
    black[0] = 0;
    black[1] = 0;
    black[2] = 0;
    half[0] = 0;
    half[1] = 0;
    half[2] = 0;
    row = 0;
    do {
      orange[3] = (unsigned char)alpha;
      black[3] = (unsigned char)alpha;
      half[3] = (unsigned char)alpha >> 1;
      x = (float)(((int)screen_width() * 0x164) / 0x280);
      y = (text_y - 8.0f) * (float)(int)screen_height() * 0.0020833334f;
      w = (float)(int)screen_width() * 0.025f;
      h = (float)(int)screen_height() * 0.033333335f;
      draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, *(unsigned int *)black, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
      width = 0x66;
      if (row == 0) {
        y2 = text_y - 7.0f;
        draw_rect(374.0f, y2, 106.0f, 16.0f, *(unsigned int *)half);
        draw_rect(374.0f, y2, 106.0f, 3.0f, *(unsigned int *)black);
        draw_rect(374.0f, y2 + 13.0f, 106.0f, 3.0f, *(unsigned int *)black);
        draw_rect(374.0f, y2, 3.0f, 16.0f, *(unsigned int *)black);
        draw_rect(477.0f, y2, 3.0f, 16.0f, *(unsigned int *)black);
        fill = g_menu_69cfc4 * 100.0f;
        if (fill > 1.0f) {
          draw_rect(377.0f, y2 + 3.0f, fill, 10.0f, *(unsigned int *)orange);
        }
      } else if (row == 2 || row == 3) {
        width = (int)text_width(count, 0.7f);
      }
      x = (float)(((int)screen_width() * (width + 0x17e)) / 0x280);
      arrow = find_texture_dict_c("Arrow");
      draw_sprite(arrow, x, y, w, h, *(unsigned int *)black, 1);
      text_y = text_y + 30.0f;
      alpha = g_hud_69c9f4;
      row++;
    } while (row < count);
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
