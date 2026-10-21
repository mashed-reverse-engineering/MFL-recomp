#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005ddc28*/ extern float k_f_0;
/*0062a940*/ extern float g_race_update_62a940;
/*0062a944*/ extern float g_race_update_62a944;
/*0062a948*/ extern float g_race_update_62a948;
/*0062a94c*/ extern float g_race_update_62a94c;
/*0075969c*/ extern undefined4 g_fx_ptank_atomic_b;
/*007596a0*/ extern undefined4 g_particles_7596a0;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*008869cc*/ extern undefined4 g_particles_8869cc[0x1000];
/*008885a0*/ extern undefined4 g_particles_8885a0;
/* fx_ptank_b_lock_and_fill */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "locks ptank atomic b and writes position/size/colour/uv for each active fx entry (two vertices
   per entry)" */

void __cdecl fx_ptank_b_lock_and_fill(int param_1,undefined4 param_2)

{
  int iVar1;
  float fVar2;
  int *piVar3;
  RwReal RVar4;
  struct {
    float local_48;
    undefined1 *local_44;
    int local_40;
    undefined4 *local_3c;
    int local_38;
    float *local_34;
    int local_30;
    float *local_2c;
    int local_28;
    RwV3d local_24;
    float local_18;
    float local_14;
    float local_10;
    RwV3d local_c;
  } frame;
  
  frame.local_24.x = 0.0;
  frame.local_24.y = 0.0;
  frame.local_24.z = 0.0;
  if (g_particles_7596a0 != 0) {
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.local_34,1,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.local_2c,4,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.local_44,2,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.local_3c,0x80,0x40000000);
    piVar3 = g_particles_8869cc;
    frame.local_18 = *(float *)(param_1 + 0x20) * k_f_3 + *(float *)(param_1 + 0x30);
    frame.local_14 = *(float *)(param_1 + 0x24) * k_f_3 + *(float *)(param_1 + 0x34);
    frame.local_10 = *(float *)(param_1 + 0x28) * k_f_3 + *(float *)(param_1 + 0x38);
    g_particles_8885a0 = 0;
    do {
      if (piVar3[-6] != 0) {
        if ((RwMatrix *)*piVar3 == (RwMatrix *)0x0) {
          frame.local_24.x = *(float *)&piVar3[-10];
          frame.local_24.y = *(float *)&piVar3[-9];
          frame.local_24.z = *(float *)&piVar3[-8];
          frame.local_48 = float_1_0;
        }
        else {
          RwV3dTransformPoints(&frame.local_24,(RwV3d *)(piVar3 + -10),1,(RwMatrix *)*piVar3);
          iVar1 = *piVar3;
          frame.local_48 = *(float *)(iVar1 + 0x28) * *(float *)(param_1 + 0x28) +
                     *(float *)(iVar1 + 0x20) * *(float *)(param_1 + 0x20) +
                     *(float *)(iVar1 + 0x24) * *(float *)(param_1 + 0x24);
        }
        frame.local_48 = frame.local_48 * *(float *)&piVar3[-5];
        if (frame.local_48 < k_f_0) {
          frame.local_48 = 0.0;
        }
        frame.local_24.y = -frame.local_24.y;
        if (frame.local_34 != (float *)0x0) {
          frame.local_c.x = frame.local_24.x - frame.local_18;
          frame.local_c.y = frame.local_24.y - frame.local_14;
          frame.local_c.z = frame.local_24.z - frame.local_10;
          RVar4 = RwV3dNormalize(&frame.local_c,&frame.local_c);
          fVar2 = frame.local_48 + k_f_0_5;
          *frame.local_34 = RVar4 * frame.local_c.x + frame.local_18;
          frame.local_34[1] = (RVar4 * frame.local_c.y + frame.local_14) - frame.local_48 * k_f_0;
          frame.local_34[2] = RVar4 * frame.local_c.z + frame.local_10;
          frame.local_34 = (float *)((int)frame.local_34 + frame.local_30);
          *frame.local_2c = fVar2 * *(float *)piVar3[-3] * frame.local_48;
          frame.local_2c[1] = fVar2 * *(float *)piVar3[-3] * frame.local_48 * k_f_4;
          frame.local_2c = (float *)((int)frame.local_2c + frame.local_28);
          *frame.local_44 = *(undefined1 *)piVar3[-1];
          frame.local_44[1] = *(undefined1 *)(piVar3[-1] + 1);
          frame.local_44[2] = *(undefined1 *)(piVar3[-1] + 2);
          frame.local_44[3] = 0x80;
          frame.local_44 = frame.local_44 + frame.local_40;
          *frame.local_3c = *(int *)&g_race_update_62a940;
          *(float *)&frame.local_3c[1] = g_race_update_62a944;
          *(float *)&frame.local_3c[2] = g_race_update_62a948;
          *(float *)&frame.local_3c[3] = g_race_update_62a94c;
          frame.local_3c = (undefined4 *)((int)frame.local_3c + frame.local_38);
          g_particles_8885a0 = g_particles_8885a0 + 1;
          *frame.local_34 = RVar4 * frame.local_c.x + frame.local_18;
          frame.local_34[1] = (RVar4 * frame.local_c.y + frame.local_14) - frame.local_48 * k_f_0_5;
          frame.local_34[2] = RVar4 * frame.local_c.z + frame.local_10;
          frame.local_34 = (float *)((int)frame.local_34 + frame.local_30);
          *frame.local_2c = fVar2 * *(float *)piVar3[-3] * frame.local_48;
          frame.local_2c[1] = fVar2 * *(float *)piVar3[-3] * frame.local_48 * k_f_4;
          frame.local_2c = (float *)((int)frame.local_2c + frame.local_28);
          *frame.local_44 = *(undefined1 *)piVar3[-1];
          frame.local_44[1] = *(undefined1 *)(piVar3[-1] + 1);
          frame.local_44[2] = *(undefined1 *)(piVar3[-1] + 2);
          frame.local_44[3] = 0x78;
          frame.local_44 = frame.local_44 + frame.local_40;
          *frame.local_3c = *(int *)&g_race_update_62a940;
          *(float *)&frame.local_3c[1] = g_race_update_62a944;
          *(float *)&frame.local_3c[2] = g_race_update_62a948;
          *(float *)&frame.local_3c[3] = g_race_update_62a94c;
          frame.local_3c = (undefined4 *)((int)frame.local_3c + frame.local_38);
          g_particles_8885a0 = g_particles_8885a0 + 1;
        }
      }
      piVar3 = piVar3 + 0xe;
    } while ((int)piVar3 < (int)&g_particles_8885a0 + 0x2c);
    RpPTankAtomicUnlock_00545f20(g_fx_ptank_atomic_b);
    *(uint *)(*(int *)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 0x40) =
         *(uint *)(*(int *)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 0x40) | 0x800000;
    *(int *)(*(int *)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 4) = g_particles_8885a0;
  }
  return;
}


