#include "mfl_tier2.h"
/*0042bd00*/ extern char *dat_get_string();
/*005632e0*/ extern int Rt2dCTMPush();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00566060*/ extern void * __cdecl Rt2dFontFlow(void * p0, void * p1, RwReal p2, void * p3, int p4, void * p5);
/*005dd330*/ extern float float_1_0;
/*005dd624*/ extern float k_f_0_0020833334;
/*005de7c0*/ extern float k_f_0_0015625;
/*005de81c*/ extern float k_f_0_0708;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*0069ba0c*/ extern char *g_ui_font;
/*0069ba10*/ extern char *g_ui_text_brush;
static int ui_rt2d_text_map_special_chars_r(ushort * reg_eax, short * reg_ebx);
extern void ui_rt2d_text_map_special_chars(void);
#define TEXT_BUF_SIZE (512)
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

/* text_draw_box */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "Rt2dFontFlow: text flowed into a box" */

/* Draws the string string_id word-wrapped into a box with Rt2dFontFlow. Returns nothing.
 * string_id: id passed to dat_get_string to fetch the text.
 * x, y: top-left corner of the box in 640x480 layout units, y counts down from the top (inferred).
 * width, height: box size in the same units, scaled by g_ui_layout_scale_x / _y (inferred).
 * colour_rgba: colour value given to the text brush (inferred).
 * text_scale: font scale passed to Rt2dFontFlow (inferred). */
void text_draw_box(undefined4 string_id,float x,float y,float width,float height,
                  undefined4 colour_rgba,float text_scale)

{
  char *text;
  union {
    char raw_bytes[528];
    struct {
      float box_left;
      float box_bottom;
      float box_width;
      float box_height;
      undefined1 text_buf [TEXT_BUF_SIZE];
    } m;
  } frame;
  
  /* fetch the string and map its special characters into text_buf */
  text = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r((ushort *)text,(short *)frame.m.text_buf);
  Rt2dCTMPush();
  Rt2dBrushSetRGBA(g_ui_text_brush,&colour_rgba,&colour_rgba,&colour_rgba,&colour_rgba);
  /* box rectangle: x and width scale by 1/640, y and height by 1/480; the bottom edge is 1 - y/480 - height */
  frame.m.box_width = width * k_f_0_0015625 * g_ui_layout_scale_x;
  frame.m.box_height = height * k_f_0_0020833334 * g_ui_layout_scale_y;
  frame.m.box_left = x * k_f_0_0015625;
  frame.m.box_bottom = (float_1_0 - y * k_f_0_0020833334) - frame.m.box_height;
  Rt2dFontFlow((void *)g_ui_font,(void *)frame.m.text_buf,text_scale * k_f_0_0708,(void *)&frame.m.box_left,1,(void *)g_ui_text_brush);
  Rt2dCTMPop();
  return;
}


