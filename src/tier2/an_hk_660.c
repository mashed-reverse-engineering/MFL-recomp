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
/*008c86d0*/ extern undefined4 g_course_8c86d0;
#define ALPHA_OPAQUE (0xff)
#define ALPHA_CAP_FLAGGED (0x60)
#define FADE_ALPHA_RAMP_START (0x81)
#define FADE_ALPHA_OFFSET (0x80)
#define FADE_ALPHA_INPUT_MAX (0x180)
#define RGB_MASK (0xffffff)
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

/* ui_text_draw_fade_alpha */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws a dat string with a top/bottom alpha gradient from alpha (0x80..0x180 range)" */
/* Draws the dat string string_id at (x, y) in the UI font, scaled by scale (font size = scale * k_f_0_0708).
 * rgb: colour, only the low 24 bits are used. alpha: fade input, see the gradient blocks below.
 * align: passed as the third argument of ui_rt2d_text_x_offset_r (meaning inferred: text alignment).
 * Top pair of brush colours uses alpha clamped to ALPHA_OPAQUE (inferred: top vertices).
 * Bottom pair uses 0 for alpha < 0x81, (alpha + 0x80) & 0xff across the ramp (1..255 for 0x81..0x17f),
 * and ALPHA_OPAQUE for alpha >= 0x180. Leaves the texture filter set to linear. Returns nothing. */
void ui_text_draw_fade_alpha(undefined4 string_id,float x,float y,uint rgb,float scale,
               int align,int alpha)
{
  undefined4 text_ptr;
  byte alpha_byte;
  union {
    char gs_buf[268];
    struct {
      undefined4 uStack_110;
      undefined1 auStack_10c [8];
      undefined1 local_104 [256];
    } m;
  } text_scratch;
  
  text_ptr = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)text_ptr,(short *)text_scratch.m.local_104);
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
  Rt2dCTMPush();
  Rt2dCTMSetIdentity();
  ui_rt2d_text_x_offset_r((void *)text_scratch.m.local_104,(void *)text_scratch.m.auStack_10c,x,y,align,scale);
  text_scratch.m.uStack_110 = rgb;
  alpha_byte = (byte)alpha;
  /* top alpha: clamp to opaque */
  if (ALPHA_OPAQUE < alpha) {
    alpha_byte = ALPHA_OPAQUE;
  }
  rgb = CONCAT13(alpha_byte,(undefined3)rgb);
  /* bottom alpha: 0 below the ramp, (alpha + 0x80) & 0xff across it, opaque from FADE_ALPHA_INPUT_MAX */
  if (alpha < FADE_ALPHA_RAMP_START) {
    text_scratch.m.uStack_110 = text_scratch.m.uStack_110 & RGB_MASK;
  }
  else if (alpha < FADE_ALPHA_INPUT_MAX) {
    text_scratch.m.uStack_110 = CONCAT13((byte)alpha + FADE_ALPHA_OFFSET,(undefined3)rgb);
  }
  else {
    text_scratch.m.uStack_110 = CONCAT13(ALPHA_OPAQUE,(undefined3)rgb);
  }
  /* g_course_8c86d0 is a one-shot flag (meaning unknown): cap the top alpha to ALPHA_CAP_FLAGGED, then clear it */
  if (g_course_8c86d0 != 0) {
    if (ALPHA_CAP_FLAGGED < alpha_byte) {
      rgb = CONCAT13(ALPHA_CAP_FLAGGED,(undefined3)rgb);
    }
    g_course_8c86d0 = 0;
  }
  Rt2dBrushSetRGBA(g_ui_text_brush,&rgb,&rgb,&text_scratch.m.uStack_110,&text_scratch.m.uStack_110);
  Rt2dFontShow((void *)g_ui_font,(void *)text_scratch.m.local_104,scale * k_f_0_0708,(void *)text_scratch.m.auStack_10c,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


