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

void __cdecl fx_particle_spawn_from_style(float *style_inst)

{
  undefined4 *pos_ptr;
  int style_index;
  int jitter;
  uint channel;
  int channel_sum;
  int style_off;
  double rnd;
  float brightness;
  double alpha;
  struct {
    undefined4 spawn_pos_x;
    undefined4 spawn_pos_y;
    undefined4 spawn_pos_z;
    char col_r;
    char col_g;
    undefined1 col_b;
    undefined1 col_a;
    float size_x;
    float size_y;
    float vel_x;
    float vel_y;
    float vel_z;
    undefined4 accel_x;
    undefined4 accel_y;
    undefined4 accel_z;
    float life_scale;
    undefined1 field_34[4];
    undefined4 field_38;
    undefined1 field_3c[4];
    undefined4 flags_word;
    undefined1 flag_44[1];
    undefined1 flags;
  } frame;
  
  style_off = *(int *)&style_inst[7] * 0x44;      /* style record: 0x44 bytes each, colour bytes at +0x34 */
  rnd = (double)rp_random_float_range(0,0x3f800000);
  jitter = (int)(rnd * (double)*(int *)((char *)&g_effects_update_76df68 + style_off));      /* brightness jitter */
  brightness = style_inst[8];      /* clamped to the style's range */
  if (*(float *)((char *)&g_effects_update_76df6c + style_off) < brightness) {
    brightness = *(float *)((char *)&g_effects_update_76df6c + style_off);
  }
  if (brightness < *(float *)((char *)&g_effects_update_76df70 + style_off)) {
    brightness = *(float *)((char *)&g_effects_update_76df70 + style_off);
  }
  pos_ptr = *(undefined4 **)&style_inst[6];
  frame.spawn_pos_x = *pos_ptr;
  frame.spawn_pos_y = pos_ptr[1];
  frame.spawn_pos_z = pos_ptr[2];
  channel = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df34 + style_off) * (double)brightness);
  channel_sum = (channel & 0xff) + jitter;
  if ((channel_sum < 0) || (0xff < channel_sum)) {
    frame.col_r = (channel_sum < 0x100) + -1;
  }
  else {
    frame.col_r = (char)channel_sum;
  }
  channel = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df35 + style_off) * (double)brightness);
  channel_sum = (channel & 0xff) + jitter;
  if ((channel_sum < 0) || (0xff < channel_sum)) {
    frame.col_g = (channel_sum < 0x100) + -1;
  }
  else {
    frame.col_g = (char)channel_sum;
  }
  channel = (int)((double)(uint)*(byte *)((char *)&g_effects_update_76df36 + style_off) * (double)brightness);
  channel = (channel & 0xff) + jitter;
  if (((int)channel < 0) || (0xff < (int)channel)) {
    channel = (uint)(byte)(((int)channel < 0x100) - 1);
  }
  frame.col_b = (undefined1)channel;
  if ((&g_effects_update_76df37)[style_off] == 0xff) {
    frame.col_a = 0xff;
  }
  else {
    alpha = (double)(uint)(byte)(&g_effects_update_76df37)[style_off];
    frame.col_a = (int)(alpha * (double)*style_inst * (double)k_f_0_5 + alpha * (double)k_f_0_5);
  }
  frame.field_38 = *(undefined4 *)((char *)&g_effects_update_76df54 + style_off);
  frame.life_scale = *(float *)((char *)&g_effects_update_76df38 + style_off) / *style_inst;
  frame.accel_x = *(undefined4 *)((char *)&g_effects_update_76df48 + style_off);
  frame.accel_y = *(undefined4 *)((char *)&g_effects_update_76df4c + style_off);
  frame.accel_z = *(undefined4 *)((char *)&g_effects_update_76df50 + style_off);
  frame.flags_word = *(undefined4 *)((char *)&g_effects_update_76df64 + style_off);
  style_index = *(int *)&style_inst[7];
  rnd = (double)rp_random_float_range(0xbe4ccccd,0x3e4ccccd);
  frame.flags = (&g_effects_update_76df60)[style_index * 0x44];
  rnd = (rnd + (double)*(float *)((char *)&g_effects_update_76df58 + style_index * 0x44)) *
          (double)*style_inst;
  frame.size_x = (float)rnd;
  frame.size_y = (float)rnd;
  if (g_tick_number % 3 == 0) {
    frame.flags = 2;
  }
  frame.vel_x = style_inst[3];
  frame.vel_y = style_inst[4];
  frame.vel_z = style_inst[5];
  rnd = (double)rp_random_float_range(0xbf800000,0x3f800000);
  frame.vel_x = (float)(rnd * (double)k_f_0_85 + (double)frame.vel_x);
  rnd = (double)rp_random_float_range(0,0x3f000000);
  frame.vel_y = (float)((double)k_f_1_3 - rnd);
  rnd = (double)rp_random_float_range(0xbf800000,0x3f800000);
  frame.vel_z = (float)(rnd * (double)k_f_0_85 + (double)frame.vel_z);
  fx_trail_particle_spawn_interpolated(&frame.spawn_pos_x,0,0,0);
  if (*(int *)&style_inst[9] != 0) {
    fx_dust_particle_spawn(style_inst + 10,*(undefined4 **)&style_inst[6],*(int *)&style_inst[7],brightness * k_f_0_4,*(undefined4 *)&style_inst[2],*(undefined4 *)&style_inst[1]);
  }
  g_effects_update_7ef784 = (float)*(int *)&style_inst[7];
  return;
}


