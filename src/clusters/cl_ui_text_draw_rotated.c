// cluster-entries: ui_text_draw_rotated
/* Installed cluster ui_text_draw_rotated (tools/cluster_install.py from output/cluster_pilot_uc3/ui_text_draw_rotated.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern void * __cdecl Rt2dFontShow(void * p0, void * p1, RwReal p2, void * p3, void * p4);
/*0042bd00*/ extern int dat_get_string();
/*00562d20*/ extern int Rt2dCTMSetIdentity();
/*00562e10*/ extern RwBool __cdecl Rt2dCTMRotate(float p0);
/*005632e0*/ extern int Rt2dCTMPush();
/*005633c0*/ extern RwBool __cdecl Rt2dCTMTranslate(float p0, float p1);
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*005ddb00*/ extern float k_f_0_025;
/*005de81c*/ extern float k_f_0_0708;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;




/* ui_text_draw_rotated */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws a dat string with a centre rotation (Rt2dCTMRotate)" */

static int ui_rt2d_text_map_special_chars_r(ushort * p0, short * p1);
extern void ui_rt2d_text_map_special_chars(void);
static int ui_rt2d_text_x_offset_r(void * p0, void * p1, float p2, float p3, int p4, float p5);
extern void ui_rt2d_text_x_offset(void);
/* (inferred) draws dat string text_id at x,y with the given scale / alignment mode (ui_rt2d_text_x_offset) and colour, rotated by angle around the text start (baseline_shift = 0.025 * scale lifts the pivot) */
void __cdecl ui_text_draw_rotated(undefined4 text_id,float x,float y,undefined4 colour,float scale, int align,float angle)

{
  double baseline_shift;
  undefined4 text_ptr;
  union {
    char gs_buf[272];
    struct {
      float origin_x;
      float origin_y;
      undefined4 text_extent_w;
      undefined4 text_extent_h;
      undefined1 text_buf [256];
    } m;
  } frame;
  
  frame.m.text_extent_w = 0;
  frame.m.text_extent_h = 0;
  text_ptr = dat_get_string(text_id);
  ui_rt2d_text_map_special_chars_r((ushort *)text_ptr,(short *)frame.m.text_buf);
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)0x2);
  Rt2dCTMPush();
  Rt2dCTMSetIdentity();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  ui_rt2d_text_x_offset_r((void *)frame.m.text_buf,(void *)&frame.m.origin_x,x,y,align,scale);
  baseline_shift = scale * k_f_0_025;
  frame.m.origin_y = baseline_shift + frame.m.origin_y;
  Rt2dCTMTranslate(frame.m.origin_x,frame.m.origin_y);
  Rt2dCTMRotate(angle);
  Rt2dCTMTranslate(0,-baseline_shift);
  Rt2dFontShow((void *)g_ui_font,(void *)frame.m.text_buf,scale * k_f_0_0708,(void *)&frame.m.text_extent_w,(void *)g_ui_text_brush);
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

/* ui_rt2d_text_x_offset_r: C -> original ui_rt2d_text_x_offset (registers edi,esi,s0,s1,s2,s3) */
__declspec(naked) int ui_rt2d_text_x_offset_r(void * p0, void * p1, float p2, float p3, int p4, float p5)
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
