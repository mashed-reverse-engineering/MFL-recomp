#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c8e0*/ extern double text_width();
/*0042cdd0*/ extern int ui_draw_texture_640_default();
/*005dd340*/ extern float k_f_0_5;
/*005ddea8*/ extern float k_f_320;
/*005de890*/ extern float k_f_28;
/*0069bb34*/ extern undefined4 g_speaker_69bb34;
/*0069bb40*/ extern undefined4 g_hud_69bb40;
/* build_info_draw_page_0 */

/* "draws the text lines 0x1ea..0x1f0 and two icons of the first info page"
 * Takes no parameters and returns nothing; all positions are fixed screen coordinates
 * (floats passed as their raw bit patterns). The two value labels (0x1eb, 0x1ec) sit
 * centred around a HUD icon, so their x positions depend on the measured text widths. */

void build_info_draw_page_0(void)

{
  float cursor_x;
  float label_x;
  double text_1eb_width;
  double text_1ec_width;
  
  ui_draw_texture_640_default(g_speaker_69bb34,0x43a00000,0x42c80000,0x43800000,0x43000000,2);
  ui_rt2d_text_draw(0x1ea,0x43a00000,0x43340000,0xffffffff,0x3f19999a,2);
  /* measure both value labels, then centre the row: label, icon gap, label (inferred) */
  text_1eb_width = (double)text_width(0x1eb,0x3f19999a);
  text_1ec_width = (double)text_width(0x1ec,0x3f19999a);
  cursor_x = (float)((double)k_f_320 -
                 (text_1ec_width + (double)(float)text_1eb_width + (double)k_f_28) * (double)k_f_0_5);
  ui_rt2d_text_draw(0x1eb,*(int *)&cursor_x,0x43480000,0xffffffff,0x3f19999a,0);
  /* advance past the first label, then draw the icon and the second label */
  cursor_x = cursor_x + (float)text_1eb_width;
  ui_draw_texture_640_default(g_hud_69bb40,*(int *)&cursor_x,0x433c0000,0x41c00000,0x41c00000,0);
  /* a float expression passed to a `()` callee would be promoted to double (8 bytes): pass the bit pattern */
  label_x = cursor_x + k_f_28;
  ui_rt2d_text_draw(0x1ec,*(int *)&label_x,0x43480000,0xffffffff,0x3f19999a,0);
  ui_rt2d_text_draw(0x1ed,0x43a00000,0x435c0000,0xffffffff,0x3f19999a,2);
  ui_rt2d_text_draw(0x1ee,0x43a00000,0x43700000,0xffffffff,0x3f19999a,2);
  ui_rt2d_text_draw(0x1ef,0x43a00000,0x43820000,0xffffffff,0x3f19999a,2);
  ui_rt2d_text_draw(0x1f0,0x43a00000,0x438c0000,0xffffffff,0x3f19999a,2);
  return;
}


