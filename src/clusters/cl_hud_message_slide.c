// cluster-entries: hud_message_slide
/* Installed cluster hud_message_slide (tools/cluster_install.py from output/cluster_pilot_uc3/hud_message_slide.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw(int text_id, float x, float y, unsigned int color, float scale, int flags);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd7e8*/ extern float k_f_60;
/*005dd7ec*/ extern float k_f_0_99899995;
/*005dd7f0*/ extern float k_f_30;
/*005dd7f4*/ extern float k_f_80;
/*005dd7f8*/ extern float k_f_110;
/*005dd7fc*/ extern float k_f_0_625;
/*005fb0b0*/ extern undefined4 g_bomb_5fb0b0;
/*005fb0b8*/ extern undefined4 g_course_5fb0b8;
/*0064d8dc*/ extern undefined4 g_bomb_64d8dc;
/*007ef7f4*/ extern float g_tick_seconds;
/* hud_message_slide */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "slides the message 0x22B in and out using a timer in the object (+0x10)" */

void hud_message_slide_n(char *in_eax, int param_2)

{
  float fVar1;
  float local_c;
  
  fVar1 = *(float *)(in_eax + 0x10) * k_f_0_625;
  *(float *)(in_eax + 0x10) = *(float *)(in_eax + 0x10) - g_tick_seconds;
  local_c = 80.0;
  if (fVar1 < k_f_0_5) {
    fVar1 = float_1_0 - fVar1;
    local_c = (float_1_0 - (fVar1 + fVar1)) * k_f_110 + k_f_80;
  }
  fVar1 = local_c + k_f_3;
  ui_rt2d_text_draw(0x22b,323.0,fVar1,0xff000000,1.2,2);
  ui_rt2d_text_draw(0x22b,320.0,local_c,0xc800d805,1.2,2);
  ui_rt2d_text_draw(0x22c,323.0,fVar1 + k_f_30,0xff000000,0.9,2);
  ui_rt2d_text_draw(0x22c,320.0,local_c + k_f_30,0xc800d805,0.9,2);
  if ((param_2 == 0) && ((float)(int)g_bomb_5fb0b0 == k_f_0_99899995)) {
    ui_rt2d_text_draw(0x22d,323.0,fVar1 + k_f_60,0xff000000,0.9,2);
    ui_rt2d_text_draw(0x22d,320.0,local_c + k_f_60,0xc800d8d8,0.9,2);
  }
  if (((&g_bomb_64d8dc)[param_2 * 5] == g_course_5fb0b8) && ((float)(int)g_bomb_5fb0b0 == k_f_0_99899995)
     ) {
    ui_rt2d_text_draw(0x22e,323.0,fVar1 + k_f_60,0xff000000,0.9,2);
    ui_rt2d_text_draw(0x22e,320.0,local_c + k_f_60,0xc800d8d8,0.9,2);
  }
  return;
}

/* entry thunk at the original address of hud_message_slide: register inputs (eax,s0) -> natural C hud_message_slide_n; ret 0 */
__declspec(naked) void hud_message_slide(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        call hud_message_slide_n
        add esp, 8
        ret
    }
}
