#include "mfl_tier2.h"
extern void * __cdecl Rt2dFontShow(void * p0, void * p1, RwReal p2, void * p3, void * p4);
/*0042bd00*/ extern int dat_get_string();
/*00562d20*/ extern int Rt2dCTMSetIdentity();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
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

static int ui_rt2d_text_x_offset_r(void *reg_edi, void *reg_esi, float stk0, float stk1, int stk2, float stk3);
extern void ui_rt2d_text_x_offset(void);
__declspec(naked) int ui_rt2d_text_x_offset_r(void *reg_edi, void *reg_esi, float stk0, float stk1, int stk2, float stk3)
{
    __asm {
        push esi
        push edi
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        mov edi, dword ptr [esp + 28]
        mov esi, dword ptr [esp + 32]
        call ui_rt2d_text_x_offset
        add esp, 16
        pop edi
        pop esi
        ret
    }
}

/* ui_rt2d_text_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "dat_get_string + Rt2d font flow (205 callers): main frontend text draw" */

/* Draws one string of frontend UI text with the UI font.
 * string_id: key for dat_get_string, the text to draw.
 * pos_x, pos_y: text position (inferred: passed to ui_rt2d_text_x_offset).
 * rgba: colour, its address is passed to Rt2dBrushSetRGBA (inferred).
 * font_size: multiplied by k_f_0_0708 for Rt2dFontShow (inferred: font scale).
 * align: passed to ui_rt2d_text_x_offset (inferred).
 * Returns nothing. Leaves the texture filter set to linear.
 */
void ui_rt2d_text_draw(undefined4 string_id,float pos_x,float pos_y,undefined4 rgba,
                      float font_size,int align)

{
  undefined4 raw_text;
  union {
    char gs_buf[264];
    struct {
      undefined1 auStack_10c [8];
      undefined1 local_104 [256];
    } m;
  } frame;
  
  /* look up the string and convert its special characters into the local buffer */
  raw_text = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)raw_text,(short *)frame.m.local_104);
  /* linear texture filtering for the font */
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
  /* push a clean transform (identity) for the text */
  Rt2dCTMPush();
  Rt2dCTMSetIdentity();
  Rt2dBrushSetRGBA(g_ui_text_brush,&rgba,&rgba,&rgba,&rgba);
  /* lay out the text; the 8 bytes at the start of frame receive the position (inferred) */
  ui_rt2d_text_x_offset_r((void *)frame.m.local_104,(void *)frame.m.auStack_10c,pos_x,pos_y,align,font_size);
  /* draw the string with the UI font, then restore the transform */
  Rt2dFontShow((void *)g_ui_font,(void *)frame.m.local_104,font_size * k_f_0_0708,(void *)frame.m.auStack_10c,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


