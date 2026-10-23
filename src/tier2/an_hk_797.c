#include "mfl_tier2.h"
/*0040f700*/ extern double intro_banner_get_x();
/*0040f710*/ extern int intro_banner_get_flag();
/*0042c440*/ extern int ui_rt2d_text_draw(undefined4 p0, float p1, float p2, undefined4 p3, float p4, undefined4 p5);
/*0042e740*/ extern double intro_banner_slide_y();
/*00432060*/ extern int team_names_string_build();
/*00434a10*/ extern int is_team_mode();
/*005dd32c*/ extern float k_f_3;
/*005de838*/ extern float k_f_640;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
#define TEAM_NAME_TEXT_BASE (0x7c) /* (inferred) text id of team name = base + team index */
#define TEAM_LABEL_TEXT_STATE0 (0xa7) /* (inferred) label text used when slot state == 0 */
#define TEAM_LABEL_TEXT_STATE_NONZERO (0xa8) /* (inferred) label text used when slot state != 0 */
#define TEAM_TEXT_SHADOW_Y_BOTTOM (403.0f)
#define TEAM_TEXT_Y_BOTTOM (400.0f)
#define TEAM_TEXT_SHADOW_Y_TOP (93.0f)
#define TEAM_TEXT_Y_TOP (90.0f)
#define COLOUR_OPAQUE_BLACK (0xff000000)
/* intro_banner_draw_team_text */

/*
 * Draws the intro banner's team label with a black drop shadow (offset +k_f_3 in x, +3 in y).
 * Outside team mode the label is the team name (TEAM_NAME_TEXT_BASE + team); in team mode it is
 * TEAM_LABEL_TEXT_STATE0 or TEAM_LABEL_TEXT_STATE_NONZERO, chosen by the slot state. (was: "draws the intro
 * banner team name (or slot string 0xA7/0xA8) with a drop shadow")
 * colour_ptr: points at the label colour (ARGB) (inferred).
 * bottom_layout: nonzero draws the label at y 400 starting at x = slide_y and returns (inferred);
 * zero draws it at y 90 right-aligned to x 640 (inferred).
 * Returns nothing.
 */
void intro_banner_draw_team_text(undefined4 *colour_ptr,int bottom_layout)

{
  float slide_y;
  int text_id;
  int team_mode;
  double scratch_d;
  undefined4 colour;

  /* text_id starts as the banner slot index, then becomes that slot's team (inferred) */
  text_id = intro_banner_get_flag();
  text_id = g_player_slots[text_id].team;
  /* slide_y: horizontal slide of the banner, used as the label x (inferred); scratch_d is a double temporary */
  scratch_d = (double)intro_banner_slide_y();
  slide_y = (float)scratch_d;
  /* team_mode: nonzero when team play is on; reused below as the slot index (inferred) */
  team_mode = is_team_mode();
  if (team_mode == 0) {
    text_id = text_id + TEAM_NAME_TEXT_BASE;
    /* bottom layout: shadow at (slide_y + 3, 403), label at (slide_y, 400), then return */
    if (bottom_layout != 0) {
      ui_rt2d_text_draw(text_id,slide_y + k_f_3,TEAM_TEXT_SHADOW_Y_BOTTOM,COLOUR_OPAQUE_BLACK,1.0f,1);
      ui_rt2d_text_draw(text_id,slide_y,TEAM_TEXT_Y_BOTTOM,*colour_ptr,1.0f,1);
      return;
    }
    /* top layout: scratch_d is the label width (inferred); shadow at (640 - width + 3, 93) */
    scratch_d = (double)intro_banner_get_x(0x42ba0000,0xff000000,0x3f800000,2);
    ui_rt2d_text_draw(text_id,(float)(((double)k_f_640 - scratch_d) + (double)k_f_3), TEAM_TEXT_SHADOW_Y_TOP, COLOUR_OPAQUE_BLACK, 1.0f, 0x2);
    colour = *colour_ptr;
  }
  else {
    /* team mode: rebuild both team name strings, then pick the label by slot state */
    team_names_string_build(0);
    team_names_string_build(1);
    text_id = TEAM_LABEL_TEXT_STATE0;
    team_mode = intro_banner_get_flag();
    if (g_player_slots[team_mode].state != 0) {
      text_id = TEAM_LABEL_TEXT_STATE_NONZERO;
    }
    if (bottom_layout != 0) {
      ui_rt2d_text_draw(text_id,slide_y + k_f_3,TEAM_TEXT_SHADOW_Y_BOTTOM,COLOUR_OPAQUE_BLACK,1.0f,1);
      ui_rt2d_text_draw(text_id,slide_y,TEAM_TEXT_Y_BOTTOM,*colour_ptr,1.0f,1);
      return;
    }
    scratch_d = (double)intro_banner_get_x(0x42ba0000,0xff000000,0x3f800000,2);
    ui_rt2d_text_draw(text_id,(float)(((double)k_f_640 - scratch_d) + (double)k_f_3), TEAM_TEXT_SHADOW_Y_TOP, COLOUR_OPAQUE_BLACK, 1.0f, 0x2);
    colour = *colour_ptr;
  }
  /* main label, both layouts: at y 90, right-aligned to x 640 (inferred), in the colour loaded above */
  scratch_d = (double)intro_banner_get_x(0x42b40000,colour,0x3f800000,2);
  ui_rt2d_text_draw(text_id,(float)((double)k_f_640 - scratch_d), TEAM_TEXT_Y_TOP, *(int *)((char *)colour_ptr + 0), 1.0f, 0x2);
  return;
}


