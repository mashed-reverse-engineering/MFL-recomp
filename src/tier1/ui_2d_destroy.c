#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00563160*/ extern int Rt2dClose();
/*00564ac0*/ extern int Rt2dBrushDestroy();
/*005662e0*/ extern int Rt2dFontDestroy();
/*0069ba08*/ extern undefined4 g_ui_font_texdict;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*0069ba14*/ extern undefined4 g_ui_brush_14;
/*0069ba18*/ extern undefined4 g_ui_brush_transparent_black;
/*0069ba1c*/ extern undefined4 g_ui_brush_white;


/* ui_2d_destroy */

/* "destroys the Rt2d font and brushes" */

void ui_2d_destroy(void)
{
  Rt2dFontDestroy(g_ui_font);
  g_ui_font = 0;
  Rt2dBrushDestroy(g_ui_brush_transparent_black);
  Rt2dBrushDestroy(g_ui_brush_white);
  Rt2dBrushDestroy(g_ui_text_brush);
  Rt2dBrushDestroy(g_ui_brush_14);
  if (g_ui_font_texdict != (RwTexDictionary *)0x0) {
    RwTexDictionaryDestroy_004d5ac0(g_ui_font_texdict);
  }
  Rt2dClose();
  return;
}
