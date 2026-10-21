// cluster-entries: trail_particle_add
/* Installed cluster trail_particle_add (tools/cluster_install.py from output/cluster_pilot_uc3/trail_particle_add.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd330*/ extern float float_1_0;
/*005ddb18*/ extern float float_0_8;
/*005ddc28*/ extern float k_f_0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00843ea0*/ extern undefined4 g_collision_triangles;
/*00843ea4*/ extern undefined4 g_trail_843ea4;
/*00843ea8*/ extern undefined4 g_camera_843ea8;
/*00843eac*/ extern undefined4 g_trail_843eac;
/*00843eb0*/ extern undefined4 g_trail_843eb0;
/*00843eb4*/ extern undefined g_trail_particle_843eb4;
/*00843eb8*/ extern undefined g_trail_particle_843eb8;
/*00843ebc*/ extern undefined4 g_trail_843ebc;
/*00843ec0*/ extern undefined4 g_trail_843ec0;
/*00843ec4*/ extern undefined4 g_trail_843ec4;
/*00843ec8*/ extern undefined4 g_trail_843ec8;
/*00843ecc*/ extern undefined4 g_trail_843ecc;
/*00843ed0*/ extern undefined g_trail_particle_843ed0;
/*00843ed4*/ extern undefined g_trail_particle_843ed4;
/*00843ed8*/ extern undefined4 g_trail_843ed8;
/*00843edc*/ extern undefined4 g_trail_843edc;
/*00843ee0*/ extern undefined g_trail_particle_843ee0;
/*00843ee4*/ extern undefined g_trail_particle_843ee4;
/*00843ee8*/ extern undefined g_trail_particle_843ee8;
/*00843eec*/ extern undefined g_trail_particle_843eec;
/*00843ef0*/ extern undefined g_trail_particle_843ef0;
/*00843ef4*/ extern undefined g_trail_particle_843ef4;
/*00843ef8*/ extern undefined g_trail_particle_843ef8;
/*00843efc*/ extern undefined g_trail_843efc;
/*00843f00*/ extern undefined g_trail_particle_843f00;
/*00843f04*/ extern undefined g_trail_particle_843f04;
/*00843f08*/ extern undefined g_trail_particle_843f08;
/*00843f0c*/ extern undefined g_trail_particle_843f0c;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
/* trail_particle_add */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "adds one trail particle to the PTank buffer (inferred)" */

void trail_particle_add_n(RwV3d *in_edi, int in_eax, float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  int iVar12;
  float10 fVar13;
  undefined4 uVar13;
  int iVar14;
  struct {
    float pad_lo[11];
    RwV3d local_4c;
    float local_40;
    float local_3c;
    float local_38;
    float local_34;
    undefined1 pad_30[4];
    float local_2c;
    undefined1 pad_28[4];
    float local_24;
    float pad_20[3];
    float local_14;
    float local_10;
    undefined1 pad_c[4];
    float local_8;
    float local_4;
  } frame;
  
  *(int *)&in_edi[1].y = 1;
  iVar12 = g_trail_8d8b00;
  fVar1 = *(float *)(in_eax + 0x80);
  fVar2 = *(float *)(in_eax + 0x84);
  iVar14 = g_trail_8d8b00 * 0x70;
  fVar3 = RW_GLOBAL(0x18, float);
  fVar4 = RW_GLOBAL(0x1c, float);
  fVar5 = (float)*(int *)(*(int *)(in_eax + 0x60) + 0xc);
  fVar6 = (float)*(int *)(*(int *)(in_eax + 0x60) + 0x10);
  RwV3dTransformPoints(&frame.local_4c,in_edi,1,(RwMatrix *)(in_eax + 0x20));
  if ((frame.local_4c.z > fVar1) && (frame.local_4c.z < fVar2)) {
    fVar8 = float_1_0 / frame.local_4c.z;
    frame.local_8 = fVar8 * fVar5 * frame.local_4c.x;
    frame.local_4 = fVar8 * fVar6 * frame.local_4c.y;
    fVar13 = fVar5 * (float_1_0 / param_3) * float_0_8 * in_edi[1].x * fVar8;
    fVar10 = fVar13 * (fVar5 / fVar6);
    fVar9 = (float_1_0 / param_3) * fVar6 * float_0_8 * in_edi[1].x * fVar8;
    if ((-fVar10 < frame.local_8) &&
       (((frame.local_8 < fVar10 + fVar5 && (-fVar9 < frame.local_4)) && (frame.local_4 < fVar9 + fVar6)))) {
      frame.local_40 = in_edi[2].x;
      fVar11 = frame.local_8 - fVar10;
      fVar7 = frame.local_4 - fVar9;
      frame.local_2c = in_edi[2].z;
      frame.local_34 = in_edi[1].z;
      frame.local_8 = fVar10 + frame.local_8;
      frame.local_38 = in_edi[2].x;
      frame.local_4 = fVar9 + frame.local_4;
      frame.local_3c = in_edi[1].z;
      frame.local_24 = in_edi[2].y;
      frame.local_14 = fVar7;
      if (fVar7 < k_f_0) {
        frame.local_14 = 0.0;
        frame.local_2c = in_edi[2].y -
                   (in_edi[2].y - in_edi[2].z) * (((fVar9 + fVar9) - -fVar7) / (fVar9 + fVar9));
      }
      if (fVar6 < frame.local_4) {
        frame.local_24 = in_edi[2].y -
                   (in_edi[2].y - in_edi[2].z) * ((frame.local_4 - fVar6) / (fVar9 + fVar9));
        frame.local_4 = fVar6;
      }
      if (fVar5 < frame.local_8) {
        frame.local_3c = in_edi[1].z -
                   (in_edi[1].z - in_edi[2].x) * ((frame.local_8 - fVar5) / (fVar10 + fVar10));
        frame.local_34 = frame.local_3c;
        frame.local_8 = fVar5;
      }
      frame.local_10 = fVar11;
      if (fVar11 < k_f_0) {
        frame.local_10 = 0.0;
        frame.local_40 = in_edi[1].z -
                   (in_edi[1].z - in_edi[2].x) *
                   (((fVar10 + fVar10) - -fVar11) / (fVar10 + fVar10));
        frame.local_38 = frame.local_40;
        fVar11 = k_f_0;
      }
      *(float *)((char *)((undefined *)&g_collision_triangles) + iVar14) = fVar11;
      *(float *)((char *)((undefined *)&g_trail_843ea4) + iVar14) = frame.local_14;
      *(float *)((char *)((undefined *)&g_trail_843ebc) + iVar14) = frame.local_8;
      *(float *)((char *)((undefined *)&g_trail_843ec0) + iVar14) = frame.local_14;
      *(float *)((char *)((undefined *)&g_trail_843edc) + iVar14) = frame.local_4;
      *(float *)((char *)((undefined *)&g_trail_843ed8) + iVar14) = frame.local_10;
      *(float *)((char *)&g_trail_particle_843ef8 + iVar14) = frame.local_4;
      *(float *)((char *)&g_trail_particle_843ef4 + iVar14) = frame.local_8;
      *(float *)((char *)&g_trail_particle_843eb8 + iVar14) = frame.local_2c;
      *(float *)((char *)&g_trail_particle_843eb4 + iVar14) = frame.local_40;
      *(float *)((char *)&g_trail_particle_843ed4 + iVar14) = frame.local_2c;
      *(float *)((char *)&g_trail_particle_843ed0 + iVar14) = frame.local_3c;
      *(float *)((char *)&g_trail_particle_843ef0 + iVar14) = frame.local_24;
      *(float *)((char *)&g_trail_particle_843eec + iVar14) = frame.local_38;
      *(float *)((char *)&g_trail_particle_843f0c + iVar14) = frame.local_24;
      *(float *)((char *)&g_trail_particle_843f08 + iVar14) = frame.local_34;
      uVar13 = CONCAT31(CONCAT21(CONCAT11(*(undefined1 *)((int)&in_edi[3].x + 3),
                                          *(undefined1 *)&in_edi[3].x),
                                 *(undefined1 *)((int)&in_edi[3].x + 1)),
                        *(undefined1 *)((int)&in_edi[3].x + 2));
      (&g_trail_843eb0)[iVar12 * 0x1c] = uVar13;
      *(float *)&(&g_trail_843eac)[iVar12 * 0x1c] = fVar8;
      fVar3 = ((fVar4 - fVar3) / (fVar2 - fVar1)) * (fVar2 / *(volatile float *)&frame.local_4c.z) * (frame.local_4c.z - fVar1) +
              fVar3;
      *(float *)&(&g_camera_843ea8)[iVar12 * 0x1c] = fVar3;
      (&g_trail_843ecc)[iVar12 * 0x1c] = uVar13;
      *(float *)&(&g_trail_843ec4)[iVar12 * 0x1c] = fVar3;
      *(float *)&(&g_trail_843ec8)[iVar12 * 0x1c] = fVar8;
      *(float *)((char *)&g_trail_particle_843ee0 + iVar14) = fVar3;
      *(undefined4 *)((char *)&g_trail_particle_843ee8 + iVar14) = uVar13;
      *(float *)((char *)&g_trail_particle_843ee4 + iVar14) = fVar8;
      *(undefined4 *)((char *)&g_trail_particle_843f04 + iVar14) = uVar13;
      *(float *)((char *)&g_trail_843efc + iVar14) = fVar3;
      *(float *)((char *)&g_trail_particle_843f00 + iVar14) = fVar8;
      g_trail_8d8b00 = g_trail_8d8b00 + 1;
    }
  }
  return;
}

/* entry thunk at the original address of trail_particle_add: register inputs (edi,eax,s0) -> natural C trail_particle_add_n; ret 0 */
__declspec(naked) void trail_particle_add(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push edi
        call trail_particle_add_n
        add esp, 12
        ret
    }
}
