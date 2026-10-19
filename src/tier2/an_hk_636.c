#include "mfl_tier2.h"
/*00565f80*/ extern RwReal __cdecl Rt2dFontGetStringWidth(void * p0, void * p1, RwReal p2);
/*005de81c*/ extern float k_f_0_0708;
/*005de838*/ extern float k_f_640;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba0c*/ extern undefined4 g_ui_font;
static int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx);
extern void ui_rt2d_wide_string_copy(void);

#define TEXT_BUFFER_BYTES (1024)

__declspec(naked) int ui_rt2d_wide_string_copy_r(byte * reg_eax, ushort * reg_ecx)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call ui_rt2d_wide_string_copy
        ret
    }
}

/* ui_rt2d_text_width */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(string id, size): Rt2dFontGetStringWidth scaled to the 640 wide layout" */

/*
 * Returns the on-screen width of the wide string `string_id` in the 640-wide
 * UI layout. `size` is scaled by k_f_0_0708 (inferred: font size factor) before
 * measuring with the UI font. The measured width is divided by the current
 * layout scale (g_ui_layout_scale_x) and multiplied by k_f_640 (inferred: 640.0f),
 * which converts it to 640-wide layout units (inferred).
 */
float10 ui_rt2d_text_width(undefined4 string_id,float size)

{
  float10 width;
  undefined1 text_buffer [TEXT_BUFFER_BYTES];

  ui_rt2d_wide_string_copy_r(string_id,text_buffer);
  width = (float10)Rt2dFontGetStringWidth((void *)g_ui_font,text_buffer,size * k_f_0_0708);
  return (width / (float10)g_ui_layout_scale_x) * (float10)k_f_640;
}


