#include "mfl_tier2.h"
/*00401360*/ extern int cup_scene_update_light_colour();
/*00401410*/ extern int cup_scene_camera_attach();
/*0040f700*/ extern double intro_banner_get_x();
/*0042c440*/ extern int ui_rt2d_text_draw(undefined4 p0, float p1, float p2, undefined4 p3, float p4, undefined4 p5);
/*0042e740*/ extern double intro_banner_slide_y();
/*0042ed10*/ extern int intro_banner_start_cup(void);
/*004824a0*/ extern int draw_rect();
/*005dd32c*/ extern float k_f_3;
/*005dd7f4*/ extern float k_f_80;
/*005ddf74*/ extern float k_f_64;
/*005de290*/ extern float float_50_0;
/*005de8fc*/ extern float k_f_68;
/*005de900*/ extern float k_f_52;
/*005de904*/ extern float k_f_480;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8790*/ extern undefined4 g_intro_banner_text_id;
/*008c8794*/ extern float g_intro_banner_slide_step;
#define TEXT_WINNER_TITLE (0xe7)        /* text 231 */
#define TEXT_CONTINUE_PROMPT (0x2e)     /* text 46 */
#define TEXT_BANNER_SUBLINE (0x42)      /* text 66 (inferred) */
#define COLOUR_ARGB_BLACK (0xff000000)
/* race_winner_banner_draw */

/* "'Winner' banner with Continue prompt (texts 231, 46)" */

/* Draws the race-winner banner. The title and boxes are placed by the intro banner
 * slide offsets (intro_banner_slide_y / intro_banner_get_x) and the slide step.
 * continue_prompt_active != 0: full banner, title with shadow, black top box and
 * bottom box with white edge lines, then the Continue prompt and the banner text
 * g_intro_banner_text_id.
 * continue_prompt_active == 0: only the 'Winner' title, black shadow at y 63 and
 * white title at y 60 (inferred: intro phase, before the Continue prompt shows). */
void race_winner_banner_draw(int continue_prompt_active)

{
  float top_box_bottom_y;
  float bottom_box_y;
  double slide_offset;

  if (continue_prompt_active != 0) {
    /* Title with a shadow copy, z-write off for the boxes. */
    slide_offset = (double)intro_banner_slide_y();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    ui_rt2d_text_draw(TEXT_WINNER_TITLE,(float)slide_offset + k_f_3,373.0f,COLOUR_ARGB_BLACK,1.2f,1);
    ui_rt2d_text_draw(TEXT_WINNER_TITLE,(float)slide_offset,370.0f,COLOUR_WHITE_ARGB,1.2f,1);
    intro_banner_start_cup();
    /* Top box at the slide step (inferred: 640 x 80 black), then the bottom box
       (inferred: 640 x 80 black) with 1-pixel white edge lines at its top and at
       the bottom of the top box. */
    draw_rect(0,*(int *)&g_intro_banner_slide_step,0x44200000,0x42a00000,COLOUR_ARGB_BLACK);
    bottom_box_y = (k_f_480 - g_intro_banner_slide_step) - k_f_64;
    draw_rect(0,*(int *)&bottom_box_y,0x44200000,0x42a00000,COLOUR_ARGB_BLACK);
    draw_rect(0,*(int *)&bottom_box_y,0x44200000,0x3f800000,COLOUR_WHITE_ARGB);
    draw_rect(0,(top_box_bottom_y = g_intro_banner_slide_step + k_f_80, *(int *)&top_box_bottom_y),0x44200000,0x3f800000,COLOUR_WHITE_ARGB);
    /* Continue prompt and banner text, drawn over the boxes with depth test off. */
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    ui_rt2d_text_draw(TEXT_CONTINUE_PROMPT,64.0f,(k_f_480 - g_intro_banner_slide_step) - k_f_52,COLOUR_WHITE_ARGB,
                      0.6f,0);
    ui_rt2d_text_draw(TEXT_BANNER_SUBLINE,64.0f,g_intro_banner_slide_step + float_50_0,COLOUR_WHITE_ARGB,0.8f,
                      0);
    ui_rt2d_text_draw(g_intro_banner_text_id,64.0f,g_intro_banner_slide_step + k_f_68,
                      COLOUR_WHITE_ARGB,0.6f,0);
    cup_scene_update_light_colour();
    cup_scene_camera_attach();
    return;
  }
  /* Intro phase (inferred): title shadow at y 63, white title at y 60. */
  slide_offset = (double)intro_banner_get_x(0x427c0000,0xff000000,0x3f99999a,2);
  ui_rt2d_text_draw(TEXT_WINNER_TITLE,(float)(slide_offset + (double)k_f_3), 63.0f, COLOUR_ARGB_BLACK, 1.2f, 0x2);
  slide_offset = (double)intro_banner_get_x(0x42700000,0xffffffff,0x3f99999a,2);
  ui_rt2d_text_draw(TEXT_WINNER_TITLE,(float)slide_offset, 60.0f, COLOUR_WHITE_ARGB, 1.2f, 0x2);
  return;
}


