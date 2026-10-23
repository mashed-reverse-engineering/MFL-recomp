#include "mfl_tier2.h"
/*0042bd00*/ extern char *dat_get_string();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern char *g_ui_font;
/*0069ba10*/ extern char *g_ui_text_brush;
/*008c87b0*/ extern undefined4 g_course_8c87b0;

/* size in bytes of the scratch buffer that receives the mapped text */
#define MAPPED_TEXT_BYTES (1024)

static int ui_rt2d_text_map_special_chars_r(ushort * reg_eax, short * reg_ebx);
extern void ui_rt2d_text_map_special_chars(void);
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

/* ui_text_draw_b */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws dat string in rectangle g_course_8c87b0" */

/* Draws the dat string string_id as text in the g_course_8c87b0 rectangle, with font g_ui_font and brush g_ui_text_brush.
 * string_id: dat string id, looked up by dat_get_string.
 * colour: RGBA value for the brush, passed by address (inferred).
 * text_scale: multiplied by k_f_0_0708 to give the font flow width (inferred).
 * Returns nothing. */
void ui_text_draw_b(undefined4 string_id,undefined4 colour,float text_scale)

{
  char *dat_string;
  char mapped_text[MAPPED_TEXT_BYTES];

  /* look up the string and map its special characters into mapped_text */
  dat_string = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)dat_string,(short *)mapped_text);
  /* save the current transform, set the brush colour, and flow the text */
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  Rt2dFontFlow((void *)g_ui_font,(void *)mapped_text,text_scale * k_f_0_0708,(void *)&g_course_8c87b0,0,(void *)g_ui_text_brush);
  /* restore the transform saved by Rt2dCTMPush */
  Rt2dCTMPop();
  return;
}


