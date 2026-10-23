#include "mfl_tier2.h"
extern void * __cdecl Rt2dFontShow(void * p0, void * p1, RwReal p2, void * p3, void * p4);
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern char *g_ui_font;
/*0069ba10*/ extern char *g_ui_text_brush;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define TEXT_WIDE_BUF_BYTES (1024)
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

static int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx);
extern void ui_rt2d_wide_string_copy(void);
__declspec(naked) int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call ui_rt2d_wide_string_copy
        ret
    }
}

/* text_draw_plain */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "variant of text_draw without the identity CTM" */
/* Draws a byte string with the UI font at (x, y).
   text: NUL-terminated byte string. colour: packed colour value for the text brush (inferred).
   x, y: anchor position. scale: size factor, font size = scale * k_f_0_0708.
   x_offset_mode: passed to ui_rt2d_text_x_offset, which adjusts the start position (inferred: alignment).
   Returns nothing. */

void text_draw_plain(char *text,float x,float y,undefined4 colour,float scale
                    ,int x_offset_mode)

{
  union {
    char gs_buf[1032];
    struct {
      undefined1 auStack_40c [8];
      undefined1 auStack_404 [TEXT_WIDE_BUF_BYTES];
    } m;
  } scratch;

  /* Linear texture filtering for the glyphs. */
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
  /* Widen the byte string into the wide-char buffer. */
  ui_rt2d_wide_string_copy_r((byte *)text,(ushort *)scratch.m.auStack_404);
  /* Push the CTM; restored by Rt2dCTMPop at the end. */
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  /* Compute the start position into auStack_40c (inferred). */
  ui_rt2d_text_x_offset_r((void *)scratch.m.auStack_404,(void *)scratch.m.auStack_40c,x,y,x_offset_mode,scale);
  /* Show the wide string at the computed position. */
  Rt2dFontShow((void *)g_ui_font,(void *)scratch.m.auStack_404,scale * k_f_0_0708,(void *)scratch.m.auStack_40c,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


