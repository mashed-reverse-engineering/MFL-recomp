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
undefined4 weapon_mortar_shell_impact_check_n(int in_eax)

{
  undefined4 uVar1;
  int iVar2;
  float10 fVar3;
  float *pfVar4;
  float *pfVar5;
  struct {
    undefined1 local_b8;
    undefined1 local_b7;
    undefined1 local_b6;
    undefined1 local_b5;
    float local_b4;
    float local_b0;
    float local_ac;
    undefined4 local_a8;
    float local_a4;
    float local_a0;
    float local_9c;
    float local_98;
    float local_94;
    float local_90;
    RwV3d local_8c;
    RwMatrix local_80;
    float local_40;
    float local_3c;
    float local_38;
    undefined1 pad_34[4];
    undefined1 local_30 [40];
    undefined4 local_8;
    undefined4 local_4;
  } frame;
  
  if (((*(float *)(in_eax + 0x38) == k_f_0) && (*(float *)(in_eax + 0x3c) == k_f_0)) &&
     (*(float *)(in_eax + 0x40) == k_f_0)) {
    return 0;
  }
  frame.local_a4 = *(float *)(in_eax + 0x14);
  frame.local_a0 = *(float *)(in_eax + 0x18);
  frame.local_9c = *(float *)(in_eax + 0x1c);
  frame.local_98 = frame.local_a4 + *(float *)(in_eax + 0x38);
  pfVar5 = &frame.local_40;
  frame.local_94 = *(float *)(in_eax + 0x3c) + frame.local_a0;
  pfVar4 = &frame.local_a4;
  frame.local_90 = *(float *)(in_eax + 0x40) + frame.local_9c;
  uVar1 = weapon_get_rw_801b14();
  iVar2 = world_intersect_line(uVar1,pfVar4,pfVar5);
  if (iVar2 != 0) {
    frame.local_a8 = frame.local_8;
    frame.local_b8 = 0xff;
    frame.local_b7 = 0x80;
    frame.local_b6 = 0;
    frame.local_b5 = 0x40;
    iVar2 = rocket_surface_hit_test(&frame.local_40,(float *)(in_eax + 0x14));
    if (iVar2 == 0) {
      FUN_004c4720(&frame.local_b4,&frame.local_a4,&frame.local_98,frame.local_a8);
      frame.local_b4 = frame.local_40 * k_f_0_05 + frame.local_b4;
      frame.local_b0 = frame.local_3c * k_f_0_05 + frame.local_b0;
      frame.local_ac = frame.local_38 * k_f_0_05 + frame.local_ac;
      FUN_004c51a0(&frame.local_80,frame.local_30,&frame.local_b4);
      fVar3 = (float10)cos((float10)(2.6179938316345215));
      if ((float10)frame.local_80.at.x * k_f_0 +
          (float10)frame.local_80.at.y + (float10)frame.local_80.at.z * k_f_0 < fVar3) {
        fVar3 = (float10)rp_random_float_range(0x3f4ccccd,0x3f99999a);
        frame.local_8c.y = (RwReal)fVar3;
        frame.local_8c.x = (RwReal)fVar3;
        frame.local_8c.z = 1.0;
        RwMatrixScale(&frame.local_80,&frame.local_8c,rwCOMBINEPRECONCAT);
        particle_pool_find_oldest_or_free(0,&frame.local_80,0);
        effect_sprite_spawn(0,&frame.local_80,&frame.local_b8,0x40400000,0x40800000);
        fVar3 = (float10)rp_random_float_range(0x3ecccccd,0x3f19999a);
        fx_spawn_from_matrix_2(&frame.local_80,(frame.local_8c.x = (float)fVar3, *(int *)&frame.local_8c.x));
      }
      weapon_pickup_explode_effect_r(in_eax);
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
