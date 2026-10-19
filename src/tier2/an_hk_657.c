#include "mfl_tier2.h"
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*008c87a0*/ extern undefined4 g_course_8c87a0;
static int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx);
extern void ui_rt2d_wide_string_copy(void);
#define TEXT_BUF_BYTES (1024)
__declspec(naked) int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call ui_rt2d_wide_string_copy
        ret
    }
}

/* ui_text_draw_wide_a */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws a wide string in rectangle g_course_8c87a0" */

/*
 * Draws a wide (UTF-16) string with the UI font into the rectangle at g_course_8c87a0
 * (the global name is misleading: it is a draw rect, not a course).
 *   wide_text:  pointer to the wide string to draw; copied into text_buf first.
 *   colour:     address of the colour word passed to the brush setter (inferred: RGBA).
 *   text_scale: multiplied by k_f_0_0708 to give the font flow scale.
 * Pushes and pops the current transform around the draw.
 */
void ui_text_draw_wide_a(undefined4 wide_text,undefined4 colour,float text_scale)

{
  char text_buf[TEXT_BUF_BYTES];

  /* copy the wide string into the local buffer */
  ui_rt2d_wide_string_copy_r((byte *)wide_text,(ushort *)text_buf);
  /* save the transform, set the brush colour, draw, restore the transform */
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  Rt2dFontFlow((void *)g_ui_font,(void *)text_buf,text_scale * k_f_0_0708,(void *)&g_course_8c87a0,0,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


