// cluster-entries: menu_layout_build_a
/* Installed cluster menu_layout_build_a (tools/cluster_install.py from output/cluster_pilot_ql1/menu_layout_build_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00483510*/ extern void __cdecl gfx_draw_movie_rect_2d(float x, float y, float w, float h, int unused0, int unused1, int unused2, int unused3, void *texture, unsigned int alpha);
/*004837d0*/ extern void __cdecl ui_draw_curved_wipe(int unused0, int unused1, float position, float size, int unused2, int unused3, int unused4, int unused5, int unused6, int unused7);
/*00484180*/ extern void __cdecl ui_draw_sprite_oriented(void *texture, float x, float y, float w, float h, unsigned int colour, int index, int mode, int no_alpha);
/*0043cda0*/ extern void *__cdecl menu_texture_dict_load(int item_index);
/*007ef7f8*/ extern float g_menu_wave_time;          /* animation clock of the menu */
/*0069ca20*/ extern int g_menu_layout_69ca20;        /* picks the mode of the first sprite, advances 2 per wave step */
/*0069d660*/ extern int g_menu_layout_69d660;        /* picks the mode of the second sprite, kept odd */
/*008c86c0*/ extern int g_menu_layout_8c86c0;        /* wipe offset, shrinks 16 per frame, 0..512 */
/*008c8780*/ extern int g_menu_layout_8c8780;
/*008c8098*/ extern int g_hud_8c8098;

#define LAYOUT_SCREEN_SIZE 512.0f
#define MENU_CLOCK_RATE 60.0f               /* phase steps per clock unit */
#define SPRITE_X 288.0f
#define SPRITE_Y 64.0f
#define SPRITE_SIZE 352.0f
#define WAVE_PERIOD_MASK 0x1ff              /* the phase runs 0..511 */
#define PHASE_STEP_A 0x1e0                  /* phase at which g_menu_layout_69ca20 advances */
#define PHASE_STEP_B 0xe0                   /* phase at which g_menu_layout_69d660 advances */
#define PHASE_FADE_END 0x100
#define PHASE_FADE_LEN 0x10                 /* fade in / out over 16 phase steps, 16 alpha per step */
#define MODE_TABLE_PERIOD 6                 /* the mode table of the original is index % 6 over 16 entries */
#define MODE_TABLE_MASK 0xf
#define ANGLE_PHASE_SHIFT 0x20
#define ANGLE_UNITS 288                     /* the sprite angle (phase << 8) / 288 */
#define WIPE_OFFSET_STEP 16
#define WIPE_OFFSET_MAX 0x200

/* menu_layout_build_a: draws the menu background: the movie frame, two wave sprites cross faded by the phase of the menu clock, and the curved wipe
   that slides away with g_menu_layout_8c86c0 */
void menu_layout_build_a(void)
{
  int phase;                       /* menu clock in steps of 1/60, 0..511 */
  int alpha_a;                     /* fade of the first sprite */
  int alpha_b;                     /* fade of the second sprite */
  int mode_a, mode_b;
  int angle;
  int step;
  unsigned int colour;
  float wipe_position;

  gfx_draw_movie_rect_2d(0.0f, 0.0f, LAYOUT_SCREEN_SIZE, LAYOUT_SCREEN_SIZE, 0, g_hud_8c8098, 0, g_menu_layout_8c8780, 0, 0xff);

  phase = (int)(g_menu_wave_time * MENU_CLOCK_RATE) & WAVE_PERIOD_MASK;
  if (phase == PHASE_STEP_A) {
    g_menu_layout_69ca20 += 2;
  } else if (phase == PHASE_STEP_B) {
    g_menu_layout_69d660 += 2;
  }
  if ((g_menu_layout_69d660 & 1) == 0) {
    g_menu_layout_69d660++;
  }

  if (phase < PHASE_STEP_B) {
    alpha_b = 0;
    alpha_a = 0xff;
  } else if (phase < PHASE_FADE_END) {
    step = phase - PHASE_STEP_B;
    if (step < PHASE_FADE_LEN) {
      alpha_b = step * 16;
      alpha_a = 0xff;
    } else {
      alpha_b = 0xff;
      alpha_a = (0x1f - step) * 16;
    }
  } else if (phase < PHASE_STEP_A) {
    alpha_a = 0;
    alpha_b = 0xff;
  } else {
    step = phase - PHASE_STEP_A;
    if (step < PHASE_FADE_LEN) {
      alpha_a = step * 16;
      alpha_b = 0xff;
    } else {
      alpha_b = (0x1f - step) * 16;
      alpha_a = 0xff;
    }
  }
  mode_a = (g_menu_layout_69ca20 & MODE_TABLE_MASK) % MODE_TABLE_PERIOD;
  mode_b = (g_menu_layout_69d660 & MODE_TABLE_MASK) % MODE_TABLE_PERIOD;

  if (alpha_a != 0) {
    colour = 0x00ffffff | ((unsigned int)alpha_a << 24);
    angle = phase + ANGLE_PHASE_SHIFT;
    if (angle > WAVE_PERIOD_MASK) {
      angle -= 0x200;
    }
    ui_draw_sprite_oriented(menu_texture_dict_load(0), SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE, colour, (angle << 8) / ANGLE_UNITS, mode_a, 1);
  }
  if (alpha_b != 0) {
    colour = 0x00ffffff | ((unsigned int)alpha_b << 24);
    ui_draw_sprite_oriented(menu_texture_dict_load(1), SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE, colour, ((phase - PHASE_STEP_B) << 8) / ANGLE_UNITS, mode_b, 1);
  }

  wipe_position = (float)g_menu_layout_8c86c0 + LAYOUT_SCREEN_SIZE;
  g_menu_layout_8c86c0 -= WIPE_OFFSET_STEP;
  if (g_menu_layout_8c86c0 < 0) {
    g_menu_layout_8c86c0 = 0;
  } else if (g_menu_layout_8c86c0 > WIPE_OFFSET_MAX) {
    g_menu_layout_8c86c0 = WIPE_OFFSET_MAX;
  }
  ui_draw_curved_wipe(0, 0, wipe_position, LAYOUT_SCREEN_SIZE, 0, g_hud_8c8098, 0, g_menu_layout_8c8780, 0, 0xff);
}

