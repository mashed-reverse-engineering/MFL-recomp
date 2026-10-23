// cluster-entries: frontend_draw
/* Installed cluster frontend_draw (tools/cluster_install.py from output/cluster_pilot_uc3/frontend_draw.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/*0060a320*/ extern undefined g_menu_page_06_challenge_select;
/*0060a378*/ extern undefined g_menu_page_07_challenge_select;
/*0060a8c0*/ extern undefined g_menu_page_18_game_mode;
/*0060a990*/ extern undefined g_menu_page_24_game_mode;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];

extern int g_menu_current;
extern struct RwGlobalsRelease *RwEngineInstance;
/* DECLS-END */
extern int menu_fade_overlay_draw(int a);
extern int menu_layout_build_a(int a);
extern int credits_update_draw(void);
extern int ui_get_menu_69d1ec(void);
extern int ui_rt2d_text_draw(int id, float x, float y, unsigned int colour, float scale, int flag);
extern int ui_text_draw_fade_alpha(int id, float x, float y, unsigned int colour, float scale, int param, int counter);
extern int draw_rect(float x, float y, float w, float h, unsigned int colour);
extern int gfx_draw_rect_2d_gradient_rgb(float x, float y, float w, float h, unsigned int colour);
extern int gfx_draw_triangle_2d(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int colour);
extern int draw_sprite_uv(int texture, float x, float y, float w, float h, unsigned int colour, float u0, float v0, float u1, float v1, int a, int b);
extern int find_texture_dict_c(const char *name);
extern int hud_menu_draw_all_panels(void);
extern int g_frontend_state, g_menu_current, g_menu_start_race_requested, g_menu_69ca1c, g_course_8c86d0, g_event_type;
extern float k_f_13, k_f_2, k_f_3, float_1_0;

/* Geometry of one menu button box: the face rectangle (x, y, w, h) and the gradient strip beside it (gx, gy, gw, gh). */
typedef struct ButtonGeom {
  float x, y, w, h;
  float gx, gy, gw, gh;
} ButtonGeom;

static int course_flag_30_set_r(unsigned int * p0);
extern void course_flag_30_set(void);
/* The border of a button box that is taller than 2: the top strip, the bottom strip (n - 2 below) and the left edge (58 or x - 2). */
static void button_border(ButtonGeom *g, unsigned int colour, int left_literal)
{
  int n;
  float m;

  if (k_f_2 < g->h) {
    n = (int)g->gh;
    g->h = 2.0f;
    g->gh = 2.0f;
    draw_rect(g->x, g->y, g->w, 2.0f, colour);
    gfx_draw_rect_2d_gradient_rgb(g->gx, g->gy, g->gw, g->gh, colour);
    m = (float)(n - 2);
    g->y = m + g->y;
    g->gy = m + g->gy;
    draw_rect(g->x, g->y, g->w, g->h, colour);
    gfx_draw_rect_2d_gradient_rgb(g->gx, g->gy, g->gw, g->gh, colour);
    g->y = g->y - m;
    g->w = 2.0f;
    if (left_literal) {
      g->x = 58.0f;
    }
    else {
      g->x = g->x - k_f_2;
    }
    g->h = (float)n;
    draw_rect(g->x, g->y, g->w, g->h, colour);
  }
}

/* The two shadowed text calls of a plain text item. */
static void draw_item_text(MenuItemAnim *it, int id)
{
  if (g_event_type == EVENT_TRAINING && id == 0x27b) {
    id = 0x2b1;
  }
  ui_text_draw_fade_alpha(id, it->x + k_f_3, it->y + k_f_3, 0xff000000, it->scale, it->param, it->anim_counter);
  ui_text_draw_fade_alpha(id, it->x, it->y, it->colour, it->scale, it->param, it->anim_counter);
}

/* The text of a button item (centred label for the id 0x229, or a faded copy when g_course_8c86d0 is set). */
static void button_text(MenuItemAnim *it, int id, unsigned int *pcolour)
{
  unsigned int colour;

  *pcolour = *pcolour & 0xff000000;
  colour = *pcolour;
  if (g_course_8c86d0 == 0) {
    if (id == 0x229) {
      ui_text_draw_fade_alpha(0x229, 320.0f, it->y, colour, 0.7f, 2, it->anim_counter);
    }
    else {
      ui_text_draw_fade_alpha(id, it->x, it->y, colour, it->scale, it->param, it->anim_counter);
    }
  }
  else {
    ui_text_draw_fade_alpha(id, it->x, it->y, 0x80000000, it->scale, it->param, 0x80);
  }
}

/* Per frame render of the frontend: the render states, the credits and title, the fade overlay, then every menu item animation (plain text items, and the
   framed buttons with the highlighted / enabled state) and the panels. */
void frontend_draw(void)
{
  unsigned int colour;
  int i;
  int bn;
  int rec;
  int selected_rec;
  int row;
  int id;
  int n;
  int ff;
  MenuItemAnim *it;
  ButtonGeom g;
  unsigned int unselected_colour;

  bn = 0;
  colour = 0xffffffff;
  unselected_colour = 0x40f8d0e8;
  if (g_frontend_state == 0) {
    return;
  }
  if (g_frontend_state == 1) {
    menu_fade_overlay_draw(1);
    RwRenderStateSet(6, (void *)1);
    RwRenderStateSet(8, (void *)1);
    return;
  }
  RwRenderStateSet(6, (void *)0);
  RwRenderStateSet(8, (void *)1);
  if (g_frontend_state < 5) {
    menu_layout_build_a(0);
    credits_update_draw();
    if (ui_get_menu_69d1ec() != 0x21) {
      ui_rt2d_text_draw(0x42, 600.0f, 52.0f, 0xff000000, 0.8f, 1);
      ui_rt2d_text_draw(0x42, 596.0f, 48.0f, 0xffffffff, 0.8f, 1);
    }
  }
  menu_fade_overlay_draw(1);
  if (g_frontend_state < 4) {
    for (i = 0; i < 30; i++) {
      it = &g_menu_item_anims[i];
      id = it->item_id;
      if (id > (int)0xff110000) {
        if (id != (int)0xff120000 && id != (int)0xff130000 && id != (int)0xff230000) {
          continue;
        }
      }
      else if (id != (int)0xff110000 && id != (int)0xff000000 && id != (int)0xff100000) {
        if (id != (int)0xff040000) {
          continue;
        }
        goto button;
      }
      /* plain text item */
      if (it->state == 0 || it->state == 2) {
        if (it->ref_b == -1) {
          continue;
        }
        draw_item_text(it, it->ref_b);
      }
      if (it->state != 0 && it->state != 2 && it->ref_a != -1) {
        draw_item_text(it, it->ref_a);
      }
      continue;
button:
      /* framed button */
      bn = bn + 1;
      g.y = (it->y - k_f_13) + float_1_0;
      g.gy = g.y;
      colour = (colour & 0x00ffffff) | 0xff000000;
      g.x = 60.0f;
      if (g_menu_records[g_menu_current].item_list == (char *)&g_menu_page_18_game_mode ||
          g_menu_records[g_menu_current].item_list == (char *)&g_menu_page_24_game_mode ||
          g_menu_records[g_menu_current].item_list == (char *)&g_menu_page_06_challenge_select ||
          (g_menu_records[g_menu_current].item_list == (char *)&g_menu_page_07_challenge_select && g_menu_69ca1c != 0)) {
        g.w = 166.0f;
        g.gw = 84.0f;
        g.gx = 226.0f;
      }
      else {
        g.w = 210.0f;
        g.gx = 270.0f;
        g.gw = 100.0f;
      }
      g.gh = 26.0f;
      g.h = 26.0f;
      if (it->anim_counter < 0x1ff) {
        n = it->anim_counter / 10;
        if (n < 0xd) {
          ff = n;
          g.y = it->y - (float)ff;
          g.gy = g.y;
          g.gh = (float)ff + (float)ff;
          g.h = g.gh;
        }
      }
      if (g_menu_start_race_requested != 0) {
        row = 1;
      }
      else {
        row = ((g_menu_69ca1c != 0) - 1) & 2;
      }
      if (it->state == 0 || it->state == 2) {
        id = it->ref_b;
        if (id == -1) {
          continue;
        }
        rec = g_menu_current - row;
        if (g_menu_records[rec + 1].entry_enabled[bn - 1] == 0) {
          course_flag_30_set_r(&colour);
        }
        if (bn - 1 == g_menu_records[rec + 1].selected) {
          if (id != 0x229) {
            colour = 0xa0146ef0;
            draw_rect(g.x, g.y, g.w, g.h, colour);
            gfx_draw_rect_2d_gradient_rgb(g.gx, g.gy, g.gw, g.gh, colour);
            colour = 0xff1050b4;
            button_border(&g, colour, 1);
            draw_sprite_uv(find_texture_dict_c("Button"), g.x - k_f_13, g.y, 13.0f, g.h, colour, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
          }
          colour = (colour & 0x00ffffff) | 0xff000000;
        }
        else {
          draw_rect(g.x, g.y, g.w, g.h, unselected_colour);
          gfx_draw_rect_2d_gradient_rgb(g.gx, g.gy, g.gw, g.gh, unselected_colour);
          colour = (colour & 0xff000000) | 0x501513;
          button_border(&g, colour, 1);
        }
        button_text(it, id, &colour);
      }
      if (it->state != 0 && it->state != 2 && it->ref_a != -1) {
        id = it->ref_a;
        selected_rec = g_menu_current;
        if (g_menu_records[selected_rec].entry_enabled[bn - 1] == 0) {
          course_flag_30_set_r(&colour);
        }
        if (bn - 1 == g_menu_records[selected_rec].selected) {
          if (id == 0x229) {
            colour = 0xa0146ef0;
            gfx_draw_triangle_2d(280.0f, 0.0f, 360.0f, 0.0f, 320.0f, 40.0f, colour);
            gfx_draw_triangle_2d(280.0f, 480.0f, 360.0f, 480.0f, 320.0f, 440.0f, colour);
            gfx_draw_triangle_2d(0.0f, 200.0f, 0.0f, 280.0f, 40.0f, 240.0f, colour);
            gfx_draw_triangle_2d(640.0f, 200.0f, 640.0f, 280.0f, 600.0f, 240.0f, colour);
          }
          if (it->ref_a != 0x229) {
            colour = 0xa0146ef0;
            draw_rect(g.x, g.y, g.w, g.h, colour);
            gfx_draw_rect_2d_gradient_rgb(g.gx, g.gy, g.gw, g.gh, colour);
            colour = 0xff1050b4;
            button_border(&g, colour, 0);
            draw_sprite_uv(find_texture_dict_c("Button"), g.x - k_f_13, g.y, 13.0f, g.h, colour, 0.0f, 1.0f, 0.0f, 1.0f, 1, 0);
          }
          colour = (colour & 0x00ffffff) | 0xff000000;
        }
        else {
          draw_rect(g.x, g.y, g.w, g.h, unselected_colour);
          gfx_draw_rect_2d_gradient_rgb(g.gx, g.gy, g.gw, g.gh, unselected_colour);
          colour = (colour & 0xff000000) | 0x501513;
          button_border(&g, colour, 0);
        }
        button_text(it, it->ref_a, &colour);
      }
    }
    hud_menu_draw_all_panels();
  }
  RwRenderStateSet(6, (void *)1);
  RwRenderStateSet(8, (void *)1);
}

/* course_flag_30_set_r: C -> original course_flag_30_set (registers eax) */
__declspec(naked) int course_flag_30_set_r(unsigned int * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call course_flag_30_set
        ret
    }
}
