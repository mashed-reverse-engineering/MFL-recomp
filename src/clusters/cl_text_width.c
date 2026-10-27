// cluster-entries: text_width
/* Installed cluster text_width (tools/cluster_install.py from output/cluster_pilot_zx/text_width.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0042bd00*/ extern int dat_get_string();
/*00565f80*/ extern RwReal __cdecl Rt2dFontGetStringWidth(void * p0, void * p1, RwReal p2);
/*005de81c*/ extern float k_f_0_0708;
/*005de838*/ extern float k_f_640;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba0c*/ extern undefined4 g_ui_font;
#define TEXT_BUFFER_SIZE (1024)

/* text_width */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "width of a string at a scale (Rt2dFontGetStringWidth)" */
/* Returns the on-screen width of the string with id string_id, measured with g_ui_font.
 * font_size is scaled by k_f_0_0708 before measuring (inferred: font size in game units).
 * Special-character codes are mapped by ui_rt2d_text_map_special_chars first.
 * The raw width is divided by g_ui_layout_scale_x and multiplied by k_f_640
 * (inferred: converts to the fixed 640-wide layout, from the name of k_f_640). */

static int ui_rt2d_text_map_special_chars_r(ushort * p0, short * p1);
extern void ui_rt2d_text_map_special_chars(void);
float10 text_width(undefined4 string_id,float font_size)

{
  undefined4 string_ptr;
  float10 raw_width;
  undefined1 mapped_text [TEXT_BUFFER_SIZE];

  string_ptr = dat_get_string(string_id);
  ui_rt2d_text_map_special_chars_r(string_ptr,mapped_text);
  raw_width = (float10)Rt2dFontGetStringWidth((void *)g_ui_font,mapped_text,font_size * k_f_0_0708);
  return (raw_width / (float10)g_ui_layout_scale_x) * (float10)k_f_640;
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
