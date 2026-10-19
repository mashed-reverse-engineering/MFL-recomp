#include "mfl_tier2.h"
/*0042bd00*/ extern int dat_get_string();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005dd330*/ extern float float_1_0;
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*005de81c*/ extern float k_f_0_0708;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
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

/* ui_text_draw_boxed */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "dat string drawn in a box (x,y,w,h in 640x480 space, alignment 0/1/2)" */

/*
 * Draws the dat string string_id inside a box in 640x480 layout space.
 *   x, y        top left corner of the box, y counts down from the top
 *   w, h        box size, same units as x and y
 *   colour      RGBA value, passed to the brush for all four channels (inferred)
 *   font_scale  text size multiplier (inferred)
 *   alignment   0, 1 or 2 (inferred: left / centre / right, check the menu callers)
 *   unused_arg_9..11  not read by this function
 * Returns nothing. Draws with the g_ui_font / g_ui_text_brush globals.
 */
void __cdecl ui_text_draw_boxed(undefined4 string_id,float x,float y,float w,float h,
                  undefined4 colour,float font_scale,int alignment,char *unused_arg_9,
                  char *unused_arg_10,char *unused_arg_11)

{
  undefined4 text_ptr;
  undefined4 flow_align;
  union {
    char raw[528];
    struct {
      float left;
      float bottom;
      float width;
      float height;
      undefined1 text_buf [512];
    } box;
  } frame;

  /* map alignment to the Rt2dFontFlow flow mode: 0->0, 1->2, 2->1 (inferred, the mapping is swapped for 1 and 2) */
  flow_align = 0;
  if (alignment == 0) {
    flow_align = 0;
  }
  else if (alignment == 1) {
    flow_align = 2;
  }
  else if (alignment == 2) {
    flow_align = 1;
  }
  /* fetch the string and copy it into the frame buffer, mapping special characters */
  text_ptr = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)text_ptr,(short *)frame.box.text_buf);
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour,&colour,&colour,&colour);
  /* box to Rt2d units: width/height scaled by the layout scale, origin moved to the
     bottom left (y flipped against the 480 height) */
  frame.box.width = w * k_f_0_0015625 * g_ui_layout_scale_x;
  frame.box.height = h * k_f_0_0020833334 * g_ui_layout_scale_y;
  frame.box.left = x * k_f_0_0015625;
  frame.box.bottom = (float_1_0 - y * k_f_0_0020833334) - frame.box.height;
  Rt2dFontFlow((void *)g_ui_font,(void *)frame.box.text_buf,font_scale * k_f_0_0708,(void *)&frame.box.left,flow_align,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


