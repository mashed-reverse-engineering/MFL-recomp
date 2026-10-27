// cluster-entries: ui_text_draw_a
/* Installed cluster ui_text_draw_a (tools/cluster_install.py from output/cluster_pilot_zx/ui_text_draw_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0042bd00*/ extern int dat_get_string();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*008c87a0*/ extern undefined4 g_course_8c87a0;
#define UI_TEXT_BUFFER_SIZE (1024)

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
static int ui_rt2d_text_map_special_chars_r(ushort * p0, short * p1);
extern void ui_rt2d_text_map_special_chars(void);
void __cdecl ui_text_draw_a(undefined4 string_id,undefined4 colour,float scale)

{
  undefined4 string_ptr;
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

/* ui_rt2d_text_map_special_chars_r: C -> original ui_rt2d_text_map_special_chars (registers eax,ebx) */
__declspec(naked) int ui_rt2d_text_map_special_chars_r(ushort * p0, short * p1)
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
