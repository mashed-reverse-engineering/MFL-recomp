// cluster-entries: weapon_mortar_shell_impact_check
/* Installed cluster weapon_mortar_shell_impact_check (tools/cluster_install.py from output/cluster_pilot_h2/weapon_mortar_shell_impact_check.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(float *p0, float *p1);
/*00485660*/ extern int particle_pool_find_oldest_or_free();
/*00487670*/ extern int effect_sprite_spawn();
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4720*/ extern void FUN_004c4720(float *p0, float *p1, float *p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(undefined4 p0, float *p1, float *p2);
/*004c51a0*/ extern void FUN_004c51a0(RwMatrix *p0, void *p1, float *p2);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
extern double cos(double);
/* weapon_mortar_shell_impact_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "checks the surface under the shell (weapon_fn_4682c0 / course colour), spawns the explosion fx
   and the impact marker" */

static int weapon_pickup_explode_effect_r(int p0);
extern void weapon_pickup_explode_effect(void);
undefined4 weapon_mortar_shell_impact_check_n(int shell)

{
  undefined4 rw_world;
  int hit;
  float10 fl_tmp;
  float *seg_start;
  float *hit_buf_ptr;
  struct {
    undefined1 marker_r;
    undefined1 marker_g;
    undefined1 marker_b;
    undefined1 marker_a;
    float impact_x;
    float impact_y;
    float impact_z;
    undefined4 hit_t;
    float seg_start_x;
    float seg_start_y;
    float seg_start_z;
    float seg_end_x;
    float seg_end_y;
    float seg_end_z;
    RwV3d decal_scale;
    RwMatrix decal_matrix;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_34[4];
    undefined1 decal_aux [40];
    undefined4 hit_record_t;
    undefined4 hit_record_tail;
  } frame;
  
  /* no velocity: nothing to test */
  if (((*(float *)(shell + 0x38) == k_f_0) && (*(float *)(shell + 0x3c) == k_f_0)) &&
     (*(float *)(shell + 0x40) == k_f_0)) {
    return 0;
  }
  /* segment from the shell position (+0x14) along its velocity (+0x38) */
  frame.seg_start_x = *(float *)(shell + 0x14);
  frame.seg_start_y = *(float *)(shell + 0x18);
  frame.seg_start_z = *(float *)(shell + 0x1c);
  frame.seg_end_x = frame.seg_start_x + *(float *)(shell + 0x38);
  hit_buf_ptr = &frame.hit_normal_x;
  frame.seg_end_y = *(float *)(shell + 0x3c) + frame.seg_start_y;
  seg_start = &frame.seg_start_x;
  frame.seg_end_z = *(float *)(shell + 0x40) + frame.seg_start_z;
  rw_world = weapon_get_rw_801b14();
  hit = world_intersect_line(rw_world,seg_start,hit_buf_ptr);
  if (hit != 0) {
    frame.hit_t = frame.hit_record_t;
    /* orange impact marker colour (255,128,0, alpha 64) */
    frame.marker_r = 0xff;
    frame.marker_g = 0x80;
    frame.marker_b = 0;
    frame.marker_a = 0x40;
    hit = rocket_surface_hit_test(&frame.hit_normal_x,(float *)(shell + 0x14));
    if (hit == 0) {
      FUN_004c4720(&frame.impact_x,&frame.seg_start_x,&frame.seg_end_x,frame.hit_t);
      frame.impact_x = frame.hit_normal_x * k_f_0_05 + frame.impact_x;
      frame.impact_y = frame.hit_normal_y * k_f_0_05 + frame.impact_y;
      frame.impact_z = frame.hit_normal_z * k_f_0_05 + frame.impact_z;
      FUN_004c51a0(&frame.decal_matrix,frame.decal_aux,&frame.impact_x);
      /* decal only when matrix.at.y < cos(150 deg) (floor-like surface in the sign convention of FUN_004c51a0), random size 0.8..1.2 */
      fl_tmp = (float10)cos((float10)(2.6179938316345215));
      if ((float10)frame.decal_matrix.at.x * k_f_0 +
          (float10)frame.decal_matrix.at.y + (float10)frame.decal_matrix.at.z * k_f_0 < fl_tmp) {
        fl_tmp = (float10)rp_random_float_range(0x3f4ccccd,0x3f99999a);
        frame.decal_scale.y = (RwReal)fl_tmp;
        frame.decal_scale.x = (RwReal)fl_tmp;
        frame.decal_scale.z = 1.0;
        RwMatrixScale(&frame.decal_matrix,&frame.decal_scale,rwCOMBINEPRECONCAT);
        particle_pool_find_oldest_or_free(0,&frame.decal_matrix,0);
        effect_sprite_spawn(0,&frame.decal_matrix,&frame.marker_r,0x40400000,0x40800000);
        fl_tmp = (float10)rp_random_float_range(0x3ecccccd,0x3f19999a);
        fx_spawn_from_matrix_2(&frame.decal_matrix,(frame.decal_scale.x = (float)fl_tmp, *(int *)&frame.decal_scale.x));
      }
      weapon_pickup_explode_effect_r(shell);
      return 1;
    }
  }
  return 0;
}

/* weapon_pickup_explode_effect_r: C -> original weapon_pickup_explode_effect (registers edi) */
__declspec(naked) int weapon_pickup_explode_effect_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_pickup_explode_effect
        pop edi
        ret
    }
}

/* entry thunk at the original address of weapon_mortar_shell_impact_check: register inputs (eax) -> natural C weapon_mortar_shell_impact_check_n; ret 0 */
__declspec(naked) void weapon_mortar_shell_impact_check(void)
{
    __asm {
        push eax
        call weapon_mortar_shell_impact_check_n
        add esp, 4
        ret
    }
}
