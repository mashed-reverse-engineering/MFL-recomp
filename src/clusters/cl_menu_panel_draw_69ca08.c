// cluster-entries: menu_panel_draw_69ca08 menu_panel_draw_69ca10
/* Installed cluster menu_panel_draw_69ca08 (tools/cluster_install.py from output/cluster_pilot_pg4/menu_panel_draw_69ca08.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040c2b0*/ extern int nop_b(void);
/*0042fae0*/ extern int game_state_get_2812(void);
/*0040d0f0*/ extern void *__cdecl find_texture_dict_c(const char *name);
/*0042c440*/ extern int __cdecl ui_rt2d_text_draw(int text_id, float x, float y, unsigned int colour, float scale, int flags);
/*0042c8e0*/ extern double __cdecl text_width(int text_id, float scale);
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*00483130*/ extern void __cdecl draw_sprite(void *texture, float x, float y, float width, float height, unsigned int colour, int flags);
/*004832d0*/ extern void __cdecl draw_sprite_uv(void *texture, float x, float y, float width, float height, unsigned int colour, float z, float u0, float v0, float u1, int a, int b);
/*007d2128*/ extern char *RwEngineInstance;
/*0069cbd0*/ extern int g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069ca08*/ extern int g_menu_panel_state_69ca08;      /* 0: fading out / hidden, 1: items of the current page, else: items of the next page */
/*0069ca0c*/ extern int g_menu_69ca0c;                  /* fade counter (alpha) */
/*0069ca10*/ extern int g_menu_panel_state_69ca10;
/*0069ca14*/ extern int g_menu_69ca14;
/*005deb20*/ extern float k_f_356;
/*005ddea0*/ extern float k_f_20;
/*005deb1c*/ extern float k_f_26;
/*005de7c0*/ extern float k_f_0_0015625;
/*005ddb50*/ extern float k_f_8;
/*005dd624*/ extern float k_f_0_0020833334;
/*005ddb00*/ extern float k_f_0_025;
/*005dd334*/ extern float k_f_0_033333335;

/* the item layout table of the menu pages (stride 0x34, one entry per item count) */
typedef struct MenuPanelLayout {
  float width;                    /* 0x00 */
  unsigned char unknown_04[0xc];
  int text_id_current;            /* 0x10: text of the panel for the current page */
  int text_id_next;               /* 0x14: text of the panel for the next page */
  unsigned char unknown_18[0x1c];
} MenuPanelLayout;
/*008c8084*/ extern MenuPanelLayout g_menu_8c8084[];

#define MENU_PANEL_MIN_WIDTH 356.0f
#define MENU_PANEL_ITEM_BASE 30
#define MENU_PANEL_FADE_LIMIT 0x60
#define MENU_PANEL_ARROW_TEXT 0x5c       /* text id of the arrow label (+1 when nop_b() is nonzero) */
#define ZTEST_ENABLE rwRENDERSTATEZTESTENABLE
#define ZWRITE_ENABLE rwRENDERSTATEZWRITEENABLE

/* Panel of a menu page (twin of menu_panel_draw_69ca10: same code for another state / fade pair and another `arrow_flag` getter): draws a left arrow sprite and the
   page text with a right arrow behind it at the y of the item list's end, fading with `fade` (alpha). `state`: 0 = nothing to
   show until the fade passes 0x60, 1 = list of the current page, otherwise list of the next page. */
static int menu_count_items_r(char * p0);
extern void menu_count_items(void);
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
void menu_panel_draw(int state, int fade, int (*arrow_flag)(void))
{
  unsigned int colour;
  float width_total;
  float y_float;
  float x;
  float y;
  float w;
  float h;
  float x2;
  double sum;
  int count;
  int text_width_int;
  int text_id;
  char *item_list;

  RwRenderStateSet(ZTEST_ENABLE, 0);
  RwRenderStateSet(ZWRITE_ENABLE, 0);
  if (state == 0 && fade < MENU_PANEL_FADE_LIMIT) {
    RwRenderStateSet(ZTEST_ENABLE, (void *)1);
    RwRenderStateSet(ZWRITE_ENABLE, (void *)1);
    return;
  }
  if (state == 1) {
    item_list = g_menu_records[g_menu_current].item_list;
  } else {
    item_list = g_menu_records[g_menu_current + 1].item_list;
  }
  count = menu_count_items_r(item_list);
  colour = (unsigned int)fade << 24;
  y_float = (float)menu_item_y_position_r(count, MENU_PANEL_ITEM_BASE);
  sum = text_width(state == 1 ? g_menu_8c8084[count].text_id_current : g_menu_8c8084[count].text_id_next, 0.8f) + g_menu_8c8084[count].width;
  if (sum >= MENU_PANEL_MIN_WIDTH) {
    width_total = (float)(sum + k_f_20);
  } else {
    width_total = MENU_PANEL_MIN_WIDTH;
  }
  x = (float)((float)screen_width() * width_total * k_f_0_0015625);
  y = (float)((y_float - k_f_8) * (float)screen_height() * k_f_0_0020833334);
  w = (float)((float)screen_width() * k_f_0_025);
  h = (float)((float)screen_height() * k_f_0_033333335);
  draw_sprite_uv(find_texture_dict_c("Arrow"), x, y, w, h, colour, 0.9999f, 0.0f, 0.0f, 1.0f, 0, 0);
  text_id = (arrow_flag() != 0) + MENU_PANEL_ARROW_TEXT;
  ui_rt2d_text_draw(text_id, width_total + k_f_20, y_float, colour, 0.7f, 0);
  text_width_int = (int)text_width(text_id, 0.7f);
  x2 = (float)((float)screen_width() * ((float)text_width_int + width_total + k_f_26) * k_f_0_0015625);
  draw_sprite(find_texture_dict_c("Arrow"), x2, y, w, h, colour, 1);
  RwRenderStateSet(ZTEST_ENABLE, (void *)1);
  RwRenderStateSet(ZWRITE_ENABLE, (void *)1);
}

/* menu_panel_draw_69ca08 (00436c00) */
void menu_panel_draw_69ca08(void)
{
  menu_panel_draw(g_menu_panel_state_69ca08, g_menu_69ca0c, nop_b);
}

/* menu_panel_draw_69ca10 (00436e70) */
void menu_panel_draw_69ca10(void)
{
  menu_panel_draw(g_menu_panel_state_69ca10, g_menu_69ca14, game_state_get_2812);
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
