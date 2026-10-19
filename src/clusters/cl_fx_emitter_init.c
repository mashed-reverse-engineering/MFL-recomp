// cluster-entries: fx_emitter_init
/* Installed cluster fx_emitter_init (tools/cluster_install.py from output/cluster_pilot11/fx_emitter_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c45e0*/ extern float rp_random_float_range(float p0, float p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd628*/ extern float k_f_0_25;
/*005ddc28*/ extern float k_f_0;
/*005de47c*/ extern float k_f_n0_25;
/* fx_emitter_init */

/* WARNING: Unknown calling convention */
/* "initialises one 0x1D8 byte emitter slot from position, size and flags" */

void fx_emitter_init_n(int in_edi, float *param_2, undefined4 param_3, undefined4 param_4, float param_5, int param_6, undefined4 param_7)

{
  float *pfVar1;
  float *pfVar2;
  RwV3d *out;
  RwV3d *out_00;
  float fVar3;
  float *pfVar4;
  float fVar5;
  
  fVar3 = k_f_0;
  *(float *)(in_edi + 0xc0) = *param_2;
  *(float *)(in_edi + 0xc4) = param_2[1];
  *(float *)(in_edi + 200) = param_2[2];
  *(undefined4 *)(in_edi + 0xd0) = 0;
  *(undefined4 *)(in_edi + 0xd4) = 0;
  *(int *)(in_edi + 0xdc) = param_6;
  *(int *)(in_edi + 0xe0) = param_6;
  *(undefined4 *)(in_edi + 0xe4) = param_7;
  *(undefined4 *)(in_edi + 0xd8) = 1;
  *(undefined4 *)(in_edi + 0xe8) = 1;
  if (fVar3 < param_5) {
    *(float *)(in_edi + 0xd0) = -param_5;
  }
  pfVar4 = (float *)(in_edi + 8);
  param_6 = 4;
  do {
    pfVar1 = pfVar4 + -2;
    *pfVar1 = *param_2;
    pfVar4[-1] = param_2[1];
    *pfVar4 = param_2[2];
    pfVar2 = pfVar4 + 0x16;
    *pfVar2 = *param_2;
    pfVar4[0x17] = param_2[1];
    pfVar4[0x18] = param_2[2];
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    *pfVar1 = fVar5 + *pfVar1;
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    pfVar4[-1] = fVar5 + pfVar4[-1];
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    *pfVar4 = fVar5 + *pfVar4;
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    *pfVar2 = fVar5 + *pfVar2;
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    pfVar4[0x17] = fVar5 + pfVar4[0x17];
    fVar5 = rp_random_float_range(*(float *)(in_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(in_edi + 0xdc) * k_f_0_25);
    out = (RwV3d *)(pfVar4 + 10);
    pfVar4[0x18] = fVar5 + pfVar4[0x18];
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    out->x = fVar5;
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    pfVar4[0xb] = fVar5;
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    pfVar4[0xc] = fVar5;
    out_00 = (RwV3d *)(pfVar4 + 0x22);
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    out_00->x = fVar5;
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    pfVar4[0x23] = fVar5;
    fVar5 = rp_random_float_range(-1.0f,1.0f);
    pfVar4[0x24] = fVar5;
    RwV3dNormalize(out,out);
    RwV3dNormalize(out_00,out_00);
    pfVar4 = pfVar4 + 3;
    param_6 = param_6 + -1;
  } while (param_6 != 0);
  return;
}

/* entry thunk at the original address of fx_emitter_init: register inputs (edi,s0,s1,s2,s3,s4,s5) -> natural C fx_emitter_init_n; ret 0 */
__declspec(naked) void fx_emitter_init(void)
{
    __asm {
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push edi
        call fx_emitter_init_n
        add esp, 28
        ret
    }
}
