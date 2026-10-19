// cluster-entries: ui_draw_framed_rect
/* Installed cluster ui_draw_framed_rect (tools/cluster_install.py from output/cluster_pilot8/ui_draw_framed_rect.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster ui_draw_framed_rect: a filled rectangle (half the alpha) with a 2 pixel border (full alpha) drawn from 5 draw_rect calls: the fill,
   then the left, top, bottom and right edge strips. */
extern float k_f_2;
extern float k_f_4;
extern void draw_rect(float x, float y, float width, float height, unsigned int colour);

/* was: EAX = alpha (low byte), stack = x, y, width, height */
void ui_draw_framed_rect_n(unsigned int alpha, float x, float y, float width, float height)
{
    unsigned int fill, border;
    float left, border_width, bottom, bottom_offset;

    fill = ((unsigned int)((unsigned char)alpha >> 1) << 24) | 0x146ef0;
    draw_rect(x, y, width, height, fill);
    left = x - k_f_2;
    border = ((unsigned int)(unsigned char)alpha << 24) | 0x1050b4;
    draw_rect(left, y, 2.0f, height, border);
    border_width = width + k_f_4;
    draw_rect(left, y, border_width, 2.0f, border);
    bottom_offset = height - k_f_2;
    bottom = bottom_offset + y;
    draw_rect(left, bottom, border_width, 2.0f, border);
    draw_rect(width + left + k_f_2, bottom - bottom_offset, 2.0f, height, border);
}

/* entry thunk at the original address of ui_draw_framed_rect: register inputs (eax,s0,s1,s2,s3) -> natural C ui_draw_framed_rect_n; ret 0 */
__declspec(naked) void ui_draw_framed_rect(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push eax
        call ui_draw_framed_rect_n
        add esp, 20
        ret
    }
}
