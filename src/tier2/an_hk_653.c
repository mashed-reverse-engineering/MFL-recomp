#include "mfl_tier2.h"
/*0042bd00*/ extern char *dat_get_string();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern char *g_ui_font;
/*0069ba10*/ extern char *g_ui_text_brush;
/*008c87a0*/ extern undefined4 g_course_8c87a0;
static int ui_rt2d_text_map_special_chars_r(ushort * reg_eax, short * reg_ebx);
extern void ui_rt2d_text_map_special_chars(void);
#define UI_TEXT_BUFFER_SIZE (1024)
__declspec(naked) int ui_rt2d_text_map_special_chars_r(ushort * reg_eax, short * reg_ebx)
{
    __asm {
        push ebx
        mov eax, dword ptr [esp + 8]
        mov ebx, dword ptr [esp + 12]
        call ui_rt2d_text_map_special_chars
        pop ebx
        ret
    }
}

/* ui_text_draw_a */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws dat string (id, colour, scale) in rectangle g_course_8c87a0 with Rt2dFontFlow" */

/*
 * Draws the dat string `string_id` in the rectangle g_course_8c87a0 with Rt2dFontFlow.
 * `colour` is the colour value the brush is set from: the same address is passed as
 * all four RGBA components (inferred). `scale` is multiplied by k_f_0_0708 to give the
 * font scale. Special characters are mapped into text_buf before drawing.
 * Returns nothing.
 */
void __cdecl ui_text_draw_a(undefined4 string_id,undefined4 colour,float scale)

{
  char *string_ptr;
  char text_buf[UI_TEXT_BUFFER_SIZE];

  /* Look up the string and map its special characters into text_buf. */
  string_ptr = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)string_ptr,(short *)text_buf);
  /* Push the transform, set the brush colour, draw the text, pop the transform. */
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  Rt2dFontFlow((void *)g_ui_font,(void *)text_buf,scale * k_f_0_0708,(void *)&g_course_8c87a0,0,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


