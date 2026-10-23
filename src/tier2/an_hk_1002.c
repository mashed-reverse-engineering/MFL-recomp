#include "mfl_tier2.h"
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*0049f850*/ extern undefined fx_dust_particle_spawn(float *p1, undefined4 *p2, int p3, float p4, undefined4 p5, undefined4 p6);
/*005dd36c*/ extern float k_f_0_4;
/*004b2b48*/ extern int _ftol();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*005de200*/ extern float k_f_0_85;
/*005de3a0*/ extern float k_f_1_3;
/*0076df34*/ extern undefined g_effects_update_76df34;
/*0076df35*/ extern undefined g_effects_update_76df35;
/*0076df36*/ extern undefined g_effects_update_76df36;
/*0076df37*/ extern undefined g_effects_update_76df37;
/*0076df68*/ extern undefined g_effects_update_76df68;
/*0076df6c*/ extern undefined g_effects_update_76df6c;
/*0076df70*/ extern undefined g_effects_update_76df70;
/*005dd340*/ extern float k_f_0_5;
/*0076df38*/ extern undefined g_effects_update_76df38;
/*0076df48*/ extern undefined g_effects_update_76df48;
/*0076df4c*/ extern undefined g_effects_update_76df4c;
/*0076df50*/ extern undefined g_effects_update_76df50;
/*0076df54*/ extern undefined g_effects_update_76df54;
/*0076df58*/ extern undefined g_effects_update_76df58;
/*0076df60*/ extern undefined g_effects_update_76df60;
/*0076df64*/ extern undefined g_effects_update_76df64;
/*007ef784*/ extern float g_effects_update_7ef784;
/*007ef804*/ extern undefined4 g_tick_number;
/* fx_particle_spawn_from_style */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns a particle from a style record: colour with brightness jitter, velocity, size, flags" */

void __cdecl fx_particle_spawn_from_style(float *param_1)

{
  undefined4 *puVar1;
  int style_index;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  double fVar7;
  float brightness;
  double alpha;
  struct {
    undefined4 local_48;
    undefined4 local_44;
    undefined4 local_40;
    char local_3c;
    char local_3b;
    undefined1 local_3a;
    undefined1 local_39;
    float local_38;
    float local_34;
    float local_30;
    float local_2c;
    float local_28;
    undefined4 local_24;
    undefined4 local_20;
    undefined4 local_1c;
    float local_18;
    undefined1 pad_14[4];
    undefined4 local_10;
    undefined1 pad_c[4];
    undefined4 local_8;
    undefined1 pad_4[1];
    undefined1 local_3;
  } frame;
  
  iVar6 = *(int *)&param_1[7] * 0x44;      /* style record: 0x44 bytes each, colour bytes at +0x34 */
  fVar7 = (double)rp_random_float_range(0,0x3f800000);
  iVar3 = (int)(fVar7 * (double)*(int *)((char *)&g_effects_update_76df68 + iVar6));      /* brightness jitter */
  brightness = param_1[8];      /* clamped to the style's range */
  if (*(float *)((char *)&g_effects_update_76df6c + iVar6) < brightness) {
    brightness = *(float *)((char *)&g_effects_update_76df6c + iVar6);
  }
  if (brightness < *(float *)((char *)&g_effects_update_76df70 + iVar6)) {
    brightness = *(float *)((char *)&g_effects_update_76df70 + iVar6);
  }
  puVar1 = *(undefined4 **)&param_1[6];
  frame.local_48 = *puVar1;
  frame.local_44 = puVar1[1];
  frame.local_40 = puVar1[2];
  uVar4 = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df34 + iVar6) * (double)brightness);
  iVar5 = (uVar4 & 0xff) + iVar3;
  if ((iVar5 < 0) || (0xff < iVar5)) {
    frame.local_3c = (iVar5 < 0x100) + -1;
  }
  else {
    frame.local_3c = (char)iVar5;
  }
  uVar4 = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df35 + iVar6) * (double)brightness);
  iVar5 = (uVar4 & 0xff) + iVar3;
  if ((iVar5 < 0) || (0xff < iVar5)) {
    frame.local_3b = (iVar5 < 0x100) + -1;
  }
  else {
    frame.local_3b = (char)iVar5;
  }
  uVar4 = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df36 + iVar6) * (double)brightness);
  uVar4 = (uVar4 & 0xff) + iVar3;
  if (((int)uVar4 < 0) || (0xff < (int)uVar4)) {
    uVar4 = (uint)(byte)(((int)uVar4 < 0x100) - 1);
  }
  frame.local_3a = (undefined1)uVar4;
  if ((&g_effects_update_76df37)[iVar6] == -1) {
    frame.local_39 = 0xff;
  }
  else {
    alpha = (double)(uint)(byte)(&g_effects_update_76df37)[iVar6];
    frame.local_39 = (int)(alpha * (double)*param_1 * (double)k_f_0_5 + alpha * (double)k_f_0_5);
  }
  frame.local_10 = *(undefined4 *)((char *)&g_effects_update_76df54 + iVar6);
  frame.local_18 = *(float *)((char *)&g_effects_update_76df38 + iVar6) / *param_1;
  frame.local_24 = *(undefined4 *)((char *)&g_effects_update_76df48 + iVar6);
  frame.local_20 = *(undefined4 *)((char *)&g_effects_update_76df4c + iVar6);
  frame.local_1c = *(undefined4 *)((char *)&g_effects_update_76df50 + iVar6);
  frame.local_8 = *(undefined4 *)((char *)&g_effects_update_76df64 + iVar6);
  style_index = *(int *)&param_1[7];
  fVar7 = (double)rp_random_float_range(0xbe4ccccd,0x3e4ccccd);
  frame.local_3 = (&g_effects_update_76df60)[style_index * 0x44];
  fVar7 = (fVar7 + (double)*(float *)((char *)&g_effects_update_76df58 + style_index * 0x44)) *
          (double)*param_1;
  frame.local_38 = (float)fVar7;
  frame.local_34 = (float)fVar7;
  if (g_tick_number % 3 == 0) {
    frame.local_3 = 2;
  }
  frame.local_30 = param_1[3];
  frame.local_2c = param_1[4];
  frame.local_28 = param_1[5];
  fVar7 = (double)rp_random_float_range(0xbf800000,0x3f800000);
  frame.local_30 = (float)(fVar7 * (double)k_f_0_85 + (double)frame.local_30);
  fVar7 = (double)rp_random_float_range(0,0x3f000000);
  frame.local_2c = (float)((double)k_f_1_3 - fVar7);
  fVar7 = (double)rp_random_float_range(0xbf800000,0x3f800000);
  frame.local_28 = (float)(fVar7 * (double)k_f_0_85 + (double)frame.local_28);
  fx_trail_particle_spawn_interpolated(&frame.local_48,0,0,0);
  if (*(int *)&param_1[9] != 0) {
    fx_dust_particle_spawn(param_1 + 10,*(undefined4 **)&param_1[6],*(int *)&param_1[7],brightness * k_f_0_4,*(undefined4 *)&param_1[2],*(undefined4 *)&param_1[1]);
  }
  g_effects_update_7ef784 = (float)*(int *)&param_1[7];
  return;
}


