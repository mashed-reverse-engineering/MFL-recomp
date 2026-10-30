// cluster-entries: ui_rt2d_text_x_offset
/* Installed cluster ui_rt2d_text_x_offset (tools/cluster_install.py from output/cluster_pilot9/ui_rt2d_text_x_offset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00565f80*/ extern RwReal __cdecl Rt2dFontGetStringWidth(void * p0, void * p1, RwReal p2);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd624*/ extern float k_f_0_0020833334;
/*005ddb00*/ extern float k_f_0_025;
/*005de7c0*/ extern float k_f_0_0015625;
/*005de81c*/ extern float k_f_0_0708;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*0069ba0c*/ extern undefined4 g_ui_font;
/* ui_rt2d_text_x_offset */

/* "(scale, x, align 0..2, ...): x position of a text string for left / centre / right alignment" */

/* (inferred names) out_pos[0] = x of the text start (left 0 / centre 1 / right 2 alignment of x, in 640 wide units), out_pos[1] = y from the top of a 480 high screen; layout scale from g_ui_layout_scale_x/y */
void ui_rt2d_text_x_offset_n(undefined4 text, float *out_pos, float x, float y, int align, float scale)

{
  short screen_dim;
  float screen_w;
  float screen_h;
  float x_px;
  float text_w_px;
  float y_px;
  
  screen_dim = screen_width();
  screen_w = (float)(int)screen_dim;
  screen_dim = screen_height();
  screen_h = (float)(int)screen_dim;
  /* one product per statement: cl would reorder (w * k) * x to (k * x) * w; x_px stays in a register per branch like the original */
  if (align == 1) {
    text_w_px = Rt2dFontGetStringWidth(g_ui_font,text,scale * k_f_0_0708);
    x_px = screen_w * k_f_0_0015625;
    x_px = x_px * x;
    text_w_px = text_w_px / g_ui_layout_scale_x;
    text_w_px = text_w_px * screen_w;
    x_px = x_px - text_w_px;
  }
  else if (align == 2) {
    text_w_px = Rt2dFontGetStringWidth(g_ui_font,text,scale * k_f_0_0708);
    x_px = screen_w * k_f_0_0015625;
    x_px = x_px * x;
    text_w_px = text_w_px / g_ui_layout_scale_x;
    text_w_px = text_w_px * screen_w;
    text_w_px = text_w_px * k_f_0_5;
    x_px = x_px - text_w_px;
  }
  else {
    x_px = screen_w * k_f_0_0015625;
    x_px = x_px * x;
  }
  x_px = x_px / screen_w;
  *out_pos = x_px * g_ui_layout_scale_x;
  y_px = screen_h * k_f_0_0020833334;
  y_px = y_px * y;
  y_px = y_px / screen_h;
  out_pos[1] = (float_1_0 - y_px * g_ui_layout_scale_y) - scale * k_f_0_025;
  return;
}

/* entry thunk at the original address of ui_rt2d_text_x_offset: register inputs (edi,esi,s0,s1,s2,s3) -> natural C ui_rt2d_text_x_offset_n; ret 0 */
__declspec(naked) void ui_rt2d_text_x_offset(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push esi
        push edi
        call ui_rt2d_text_x_offset_n
        add esp, 24
        ret
    }
}
