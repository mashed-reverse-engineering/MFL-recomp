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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "(scale, x, align 0..2, ...): x position of a text string for left / centre / right alignment" */

void ui_rt2d_text_x_offset_n(undefined4 in_edi, float *in_esi, float param_3, float param_4, int param_5, float param_6)

{
  short sVar1;
  int iVar2;
  float10 fVar3;
  float10 fVar4;
  
  sVar1 = screen_width();
  iVar2 = (int)sVar1;
  sVar1 = screen_height();
  if (param_5 == 1) {
    fVar4 = (float10)Rt2dFontGetStringWidth(g_ui_font,in_edi,param_6 * k_f_0_0708);
    fVar3 = (float10)iVar2 * (float10)k_f_0_0015625 * (float10)param_3;
    fVar4 = (fVar4 / (float10)g_ui_layout_scale_x) * (float10)iVar2;
  }
  else {
    if (param_5 != 2) {
      fVar3 = (float10)iVar2 * (float10)k_f_0_0015625 * (float10)param_3;
      goto LAB_0042bcbb;
    }
    fVar4 = (float10)Rt2dFontGetStringWidth(g_ui_font,in_edi,param_6 * k_f_0_0708);
    fVar3 = (float10)iVar2 * (float10)k_f_0_0015625 * (float10)param_3;
    fVar4 = (fVar4 / (float10)g_ui_layout_scale_x) * (float10)iVar2 * (float10)k_f_0_5;
  }
  fVar3 = fVar3 - fVar4;
LAB_0042bcbb:
  *in_esi = (float)((fVar3 / (float10)iVar2) * (float10)g_ui_layout_scale_x);
  in_esi[1] = (float_1_0 -
               (((float)(int)sVar1 * k_f_0_0020833334 * param_4) / (float)(int)sVar1) *
               g_ui_layout_scale_y) - param_6 * k_f_0_025;
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
