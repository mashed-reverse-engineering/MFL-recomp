// cluster-entries: weapon_oil_slicks_update_render
/* Installed cluster weapon_oil_slicks_update_render (tools/cluster_install.py from output/cluster_pilot_j1/weapon_oil_slicks_update_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00481fd0*/ extern double random_range();
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*004868d0*/ extern int gfx_set_params_6c5124();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00498fe0*/ extern int fx_decal_stamp_matrix(RwReal *param_2, float param_3, int param_4);
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*004b2b48*/ extern int _ftol();
/*004d3d40*/ extern int MatrixOrthoNormalize_004d3d50();
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dda24*/ extern float k_f_0_75;
/*005ddc28*/ extern float k_f_0;
/*005de1d0*/ extern float k_f_1_25;
/*005de25c*/ extern float k_f_0_005;
/*005de270*/ extern float k_f_n0_1;
/*005df458*/ extern float k_f_200;
/*005df7dc*/ extern float k_f_0_995;
/*005dec28*/ extern float k_f_220;
/*005df5c4*/ extern float k_f_35;
/*005df7e4*/ extern float k_f_81;
/*005df7e0*/ extern float k_f_0_012345679;
/*005ddb28*/ extern float k_f_128;
/*005ddb0c*/ extern float k_f_15;
/*005df7e8*/ extern float k_f_n0_02;
/*005df7ec*/ extern float k_f_0_014285714;
/*006a6250*/ extern undefined4 g_weapon_oil_6a6250;
/*006a7c38*/ extern undefined g_weapon_6a7c38;
/*006a7ce8*/ extern undefined g_weapon_6a7ce8;
/*006a7d28*/ extern undefined1 g_weapon_6a7d28;
/*007ef7f4*/ extern float g_tick_seconds;
static int weapon_oil_register_trigger_sphere_r(void *slick_age_ptr);
extern void weapon_oil_register_trigger_sphere(void);
__declspec(naked) int weapon_oil_register_trigger_sphere_r(void *slick_age_ptr)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_oil_register_trigger_sphere
        ret
    }
}

/* weapon_oil_slicks_update_render */

/* "updates and renders the oil slicks: camera relative matrices, particles and the slippery area"
    */

void weapon_oil_slicks_update_render(void)

{
  float *slick_age_ptr;
  byte live_byte;
  float t_val;
  float one;
  undefined4 ortho_result;
  byte live_bit;
  RwReal *slick;
  RwReal *copy_src;
  int slot;
  int words_left_b;
  int words_left;
  int cam_matrix;
  RwMatrix *copy_dst;
  float10 rand_val;
  struct {
    undefined1 fx_color_r;
    undefined1 fx_color_g;
    undefined1 fx_color_b;
    undefined1 fx_color_a;
    undefined1 decal_color_r;
    undefined1 decal_color_g;
    undefined1 decal_color_b;
    undefined1 decal_color_a;
    undefined1 smoke_color_r;
    undefined1 smoke_color_g;
    undefined1 smoke_color_b;
    undefined1 smoke_color_a;
    float smoke_pos_x;
    RwReal smoke_pos_y;
    RwReal smoke_pos_z;
    int slot_index;
    RwV3d quad_pt0;
    float quad_pt1_x;
    undefined4 quad_pt1_y;
    float quad_pt1_z;
    float quad_pt2_x;
    undefined4 quad_pt2_y;
    float quad_pt2_z;
    float quad_pt3_x;
    undefined4 quad_pt3_y;
    float quad_pt3_z;
    RwV3d scale_vec;
    RwV3d lift_vec;
    float uv0_u;
    float uv0_v;
    float uv1_u;
    float uv1_v;
    float uv2_u;
    float uv2_v;
    float uv3_u;
    float uv3_v;
    RwMatrix slick_matrix;
    RwReal ortho_matrix [16];
  } frame;
  
  /* 26 floats per slick record (g_weapon_oil_6a6250): matrix at +0, age at -6, life fraction at -5, scale at -1; liveness bitmap g_weapon_6a7d28 */
  slot = 0;
  frame.slot_index = 0;
  slick = (RwReal *)&g_weapon_oil_6a6250;
  do {
    live_byte = (&g_weapon_6a7d28)[slot >> 3];
    live_bit = (byte)slot & 7;
    if ((live_byte & (byte)(1 << live_bit)) == 0) {
LAB_00463233:
      slot = frame.slot_index + 1;
      slick = slick + 0x1a;
      frame.slot_index = slot;
    }
    else {
      slick_age_ptr = slick + -6;
      t_val = g_tick_seconds + *slick_age_ptr;
      frame.fx_color_r = 0x80;
      frame.fx_color_g = 0x80;
      frame.fx_color_b = 0x80;
      *slick_age_ptr = t_val;
      one = float_1_0;
      frame.fx_color_a = 0xff;
      t_val = t_val * k_f_0_014285714;
      /* life fraction = age / 70 s; scale grows quickly (x200) then slowly */
      slick[-5] = t_val;
      if (!(t_val > one)) {      /* NaN stays in range (original: fcom / test ah,0x41 / jne; test ah,5 / jp) */
        if (!(t_val < k_f_0_005)) {
          t_val = t_val * k_f_0_5 + k_f_1_25;
        }
        else {
          t_val = t_val * k_f_200 * k_f_1_25;
        }
        slick[-1] = t_val;
        frame.scale_vec.x = slick[-1];
        copy_src = slick;
        copy_dst = &frame.slick_matrix;
        for (slot = 0x10; slot != 0; slot = slot + -1) {
          (copy_dst->right).x = *copy_src;
          copy_src = copy_src + 1;
          copy_dst = (RwMatrix *)&(copy_dst->right).y;
        }
        frame.scale_vec.y = frame.scale_vec.x;
        frame.scale_vec.z = frame.scale_vec.x;
        RwMatrixScale(&frame.slick_matrix,&frame.scale_vec,rwCOMBINEPRECONCAT);
        frame.fx_color_a = (byte)(int)((float_1_0 - slick[-5]) * k_f_220 + k_f_35);
        particles_set_color(&frame.fx_color_r);
        gfx_set_fx_matrix(&frame.slick_matrix);
        particles_add(&g_weapon_6a7ce8);
        copy_src = slick;
        copy_dst = &frame.slick_matrix;
        for (slot = 0x10; slot != 0; slot = slot + -1) {
          (copy_dst->right).x = *copy_src;
          copy_src = copy_src + 1;
          copy_dst = (RwMatrix *)&(copy_dst->right).y;
        }
        frame.lift_vec.x = frame.slick_matrix.at.x * k_f_n0_02;
        frame.lift_vec.y = frame.slick_matrix.at.y * k_f_n0_02;
        frame.decal_color_r = 0xff;
        frame.decal_color_g = 0xff;
        frame.lift_vec.z = frame.slick_matrix.at.z * k_f_n0_02;
        frame.decal_color_b = 0xff;
        frame.decal_color_a = 0x80;
        frame.quad_pt0.x = 0.5;
        frame.quad_pt0.y = -0.5;
        frame.quad_pt0.z = 0.0;
        frame.quad_pt1_x = -0.5;
        frame.quad_pt1_y = 0xbf000000;
        frame.quad_pt1_z = 0.0;
        frame.quad_pt2_x = -0.5;
        frame.quad_pt2_y = 0x3f000000;
        frame.quad_pt2_z = 0.0;
        frame.quad_pt3_x = 0.5;
        frame.quad_pt3_y = 0x3f000000;
        frame.quad_pt3_z = 0.0;
        RwMatrixTranslate(&frame.slick_matrix,&frame.lift_vec,rwCOMBINEPOSTCONCAT);
        cam_matrix = get_viewport_camera_matrix(0);
        {
          double dx = (double)*(float *)(cam_matrix + 0x30) - (double)frame.slick_matrix.pos.x;
          double dy = (double)*(float *)(cam_matrix + 0x34) - (double)frame.slick_matrix.pos.y;
          double dz = (double)*(float *)(cam_matrix + 0x38) - (double)frame.slick_matrix.pos.z;
          double dd = dy * dy + dx * dx + dz * dz;
          if (dd > (double)k_f_81) dd = (double)k_f_81;
          frame.decal_color_a = (byte)(int)(dd * k_f_0_012345679 * k_f_128 + k_f_15);
        }
        RwV3dTransformPoints(&frame.quad_pt0,&frame.quad_pt0,4,&frame.slick_matrix);
        {
        double t;
        #define CU (double)(*(float *)(cam_matrix + 0x30) * k_f_n0_1 - *(float *)(cam_matrix + 0x20))
        #define CV (double)(*(float *)(cam_matrix + 0x38) * k_f_n0_1 + *(float *)(cam_matrix + 0x24))
        t = (double)frame.quad_pt0.x * k_f_0_5 + CU;
        frame.uv0_u = t + k_f_0_5;
        t = (double)frame.quad_pt0.z * k_f_0_5 + CV;
        frame.uv0_v = t + k_f_0_5;
        t = (double)frame.quad_pt1_x * k_f_0_5 + CU;
        frame.uv1_u = t + k_f_0_5;
        t = (double)frame.quad_pt1_z * k_f_0_5 + CV;
        frame.uv1_v = t + k_f_0_5;
        t = (double)frame.quad_pt2_x * k_f_0_5 + CU;
        frame.uv2_u = t + k_f_0_5;
        t = (double)frame.quad_pt2_z * k_f_0_5 + CV;
        frame.uv2_v = t + k_f_0_5;
        t = (double)frame.quad_pt3_x * k_f_0_5 + CU;
        frame.uv3_u = t + k_f_0_5;
        t = (double)frame.quad_pt3_z * k_f_0_5 + CV;
        frame.uv3_v = t + k_f_0_5;
        }
        particles_set_color(&frame.decal_color_r);
        gfx_set_params_6c5124(&frame.uv0_u);
        gfx_set_fx_matrix(&frame.slick_matrix);
        particles_add(&g_weapon_6a7c38);
        weapon_oil_register_trigger_sphere_r((void *)slick_age_ptr);
        if ((*(int *)&slick[0x12] == 0) && (*(int *)&slick[0x10] == 1)) {
          t_val = slick[0x11] - g_tick_seconds;
          slick[0x11] = t_val;
          if (t_val <= k_f_0) {
            ortho_result = MatrixOrthoNormalize_004d3d50(frame.ortho_matrix,slick);
            fx_decal_stamp_matrix(frame.ortho_matrix,1.2,1);
            *(int *)&slick[0x12] = 1;
            slick[0x10] = 0.0;
            slick[0x11] = 0.1;
          }
        }
        if ((*(int *)&slick[0x12] == 1) && (slick[-5] < k_f_0_75)) {
          frame.smoke_color_r = 0x20;
          frame.smoke_color_g = 0x20;
          frame.smoke_color_b = 0x20;
          frame.smoke_color_a = 200;
          rand_val = (float10)random_range(0,0x3f800000);
          if ((float10)k_f_0_995 < rand_val) {
            frame.smoke_pos_x = slick[0xc];
            frame.smoke_pos_y = slick[0xd];
            frame.smoke_pos_z = slick[0xe];
            rand_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            frame.smoke_pos_x = (float)(rand_val + (float10)frame.smoke_pos_x);
            rand_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            frame.smoke_pos_y = (RwReal)(rand_val + (float10)frame.smoke_pos_y);
            rand_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            frame.smoke_pos_z = (RwReal)(rand_val + (float10)frame.smoke_pos_z);
            fx_smoke_particle_spawn_tinted(&frame.smoke_pos_x,0x3ecccccd,&frame.smoke_color_r,0x40a00000,0x3f000000);
          }
        }
        goto LAB_00463233;
      }
      (&g_weapon_6a7d28)[slot >> 3] = ~('\x01' << live_bit) & live_byte;
    }
    if ((int)&g_weapon_6a7c38 + 0x17 < (int)slick) {
      particles_flush(&g_weapon_6a7ce8);
      particles_flush(&g_weapon_6a7c38);
      return;
    }
  } while( true );
}

