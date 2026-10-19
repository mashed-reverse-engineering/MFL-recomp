// cluster-entries: line_of_sight_between
/* Installed cluster line_of_sight_between (tools/cluster_install.py from output/cluster_pilot9/line_of_sight_between.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(undefined4 p0, undefined4 p1);
/*00482070*/ extern double dot_product_3d_clamped();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddaec*/ extern float k_f_57_29578;
/*005ddaf0*/ extern float k_f_1_0en5;
/* line_of_sight_between */

/* WARNING: Unknown calling convention */
/* "inferred: ray test between two points with FUN_00467ea0 / FUN_004682c0" */

undefined4 line_of_sight_between_n(int in_eax, float *in_edi, float param_3, float param_4)

{
  float *pfVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  float10 fVar5;
  RwReal RVar6;
  float *pfVar7;
  undefined1 *puVar8;
  struct {
    RwV3d local_64;
    float local_58;
    float local_54;
    float local_50;
    float local_4c;
    float local_48;
    float local_44;
    undefined1 local_40 [64];
  } frame;
  
  iVar2 = *(int *)(*(char **)(in_eax + 0x10) + 4);
  pfVar1 = (float *)(iVar2 + 0x40);
  frame.local_58 = *pfVar1;
  frame.local_54 = *(float *)(iVar2 + 0x44);
  frame.local_50 = *(float *)(iVar2 + 0x48);
  frame.local_4c = *in_edi;
  frame.local_48 = in_edi[1];
  frame.local_44 = in_edi[2];
  puVar8 = frame.local_40;
  pfVar7 = &frame.local_58;
  uVar3 = weapon_get_rw_801b14();
  iVar4 = world_intersect_line(uVar3,pfVar7,puVar8);
  if ((iVar4 == 1) && (iVar4 = rocket_surface_hit_test(frame.local_40,pfVar1), iVar4 == 0)) {
    return 0;
  }
  frame.local_58 = *in_edi;
  frame.local_54 = in_edi[1];
  frame.local_50 = in_edi[2];
  frame.local_4c = *pfVar1;
  frame.local_48 = *(float *)(iVar2 + 0x44);
  frame.local_44 = *(float *)(iVar2 + 0x48);
  puVar8 = frame.local_40;
  pfVar7 = &frame.local_58;
  uVar3 = weapon_get_rw_801b14();
  iVar4 = world_intersect_line(uVar3,pfVar7,puVar8);
  if ((iVar4 == 1) && (iVar4 = rocket_surface_hit_test(frame.local_40,pfVar1), iVar4 == 0)) {
    return 0;
  }
  frame.local_64.x = *in_edi - *pfVar1;
  frame.local_64.z = in_edi[2] - *(float *)(iVar2 + 0x48);
  frame.local_64.y = 0.0;
  RVar6 = RwV3dLength(&frame.local_64);
  if (k_f_1_0en5 < frame.local_64.z * frame.local_64.z + frame.local_64.x * frame.local_64.x + frame.local_64.y * frame.local_64.y) {
    RwV3dNormalize(&frame.local_64,&frame.local_64);
  }
  fVar5 = (float10)dot_product_3d_clamped(&frame.local_64,iVar2 + 0x30);
  fVar5 = (float10)FUN_004b3284((double)fVar5);
  if (((float10)k_f_57_29578 * fVar5 < (float10)param_4 !=
       ((float10)k_f_57_29578 * fVar5 == (float10)param_4)) &&
     (RVar6 <= param_3)) {
    return 1;
  }
  return 0;
}

/* entry thunk at the original address of line_of_sight_between: register inputs (eax,edi,s0,s1) -> natural C line_of_sight_between_n; ret 0 */
__declspec(naked) void line_of_sight_between(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push edi
        push eax
        call line_of_sight_between_n
        add esp, 16
        ret
    }
}
