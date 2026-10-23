#include "mfl_tier2.h"
/*00422280*/ extern int vehicle_part_frame();
/*00422320*/ extern int vehicle_get_matrix_local();
/*00422350*/ extern char *vehicle_part_ltm();
/*00424880*/ extern int debris_apply_velocity_a(int emitter, float *velocity);
/*00424930*/ extern undefined __cdecl debris_apply_velocity_b(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, void *p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, void *p1);
/*00475730*/ extern int get_viewport_camera();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00485660*/ extern int particle_pool_find_oldest_or_free();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00498f40*/ extern int fx_ring_add_point();
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4720*/ extern void FUN_004c4720(void *p0, void *p1, void *p2, undefined4 p3);
/*004c4c30*/ extern int FUN_004c4c30(undefined4 p0, void *p1, void *p2);
/*004c51a0*/ extern void FUN_004c51a0(void *p0, undefined4 p1, void *p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*005dd36c*/ extern float k_f_0_4;
/*005dd628*/ extern float k_f_0_25;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb18*/ extern float float_0_8;
/*005ddc28*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de3e4*/ extern float k_f_40;
/*005de47c*/ extern float k_f_n0_25;
/*005de480*/ extern float k_f_n0_4;
static int rwp_world_current_activate_r(undefined4 reg_eax);
extern void rwp_world_current_activate(void);
__declspec(naked) int rwp_world_current_activate_r(undefined4 reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rwp_world_current_activate
        ret
    }
}

/* vehicle_smoke_emitters_spawn */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns smoke emitters at +/-0.4 side offsets of the vehicle (from its local matrix and velocity)
   with a random size" */

void vehicle_smoke_emitters_spawn(char *param_1)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  RwV3d *out;
  byte *pbVar1;
  undefined4 uVar2;
  int iVar3;
  RwMatrix *m;
  int camera;
  int iVar4;
  float *pfVar5;
  double fVar6;
  float fVar7;
  int emitter;
  float *pfVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  struct {
    RwV3d local_128;
    float local_11c;
    float local_118;
    float local_114;
    float local_110;
    float local_10c;
    float local_108;
    float local_104;
    float local_100;
    float local_fc;
    int local_f8;
    float local_f4;
    float local_f0;
    float local_ec;
    float local_e8;
    float local_e4;
    float local_e0;
    int local_dc;
    RwV3d local_d8;
    undefined1 local_cc [32];
    float local_ac;
    float local_a8;
    float local_a4;
    undefined1 pad_a0[4];
    float local_9c;
    float local_98;
    float local_94;
    undefined1 pad_90[4];
    float local_8c;
    float local_88;
    float local_84;
    undefined1 pad_80[4];
    undefined1 local_7c [40];
    undefined4 local_54;
    undefined1 pad_50[4];
    undefined1 local_4c [12];
    RwMatrix local_40;
  } frame;
  
  camera = *(int *)(param_1 + 0xfc);
  frame.local_128.x = 0.0;
  frame.local_128.y = 0.0;
  frame.local_128.z = 0.0;
  frame.local_dc = camera;
  vehicle_get_matrix_local(*(undefined4 *)(camera + 0x204),frame.local_cc);
  car_get_velocity_any_mode(frame.local_4c,*(undefined4 *)(*(char **)(param_1 + 0xfc) + 0x204));
  iVar3 = *(int *)(param_1 + 0xf8);
  frame.local_11c = 0.0;
  frame.local_118 = 1.0;
  frame.local_114 = 0.0;
  fVar7 = k_f_n0_4;
  if (iVar3 == 0) {
    fVar7 = k_f_0_4;
  }
  frame.local_110 = fVar7 * frame.local_ac + frame.local_9c;
  fVar7 = k_f_n0_4;
  if (iVar3 == 0) {
    fVar7 = k_f_0_4;
  }
  frame.local_10c = fVar7 * frame.local_a8 + frame.local_98;
  fVar7 = k_f_n0_4;
  if (iVar3 == 0) {
    fVar7 = k_f_0_4;
  }
  frame.local_108 = fVar7 * frame.local_a4 + frame.local_94;
  fVar6 = (double)rp_random_float_range(0x3f4ccccd,0x3f800000);
  fx_emitter_spawn_b_default(&frame.local_110,&frame.local_11c,0x41f00000,(fbits1 = (float)fVar6, *(int *)&fbits1));
  frame.local_f4 = frame.local_11c * k_f_0_125 + frame.local_110;
  frame.local_f0 = frame.local_118 * k_f_0_125 + frame.local_10c;
  pfVar8 = &frame.local_8c;
  pfVar5 = &frame.local_f4;
  frame.local_ec = frame.local_114 * k_f_0_125 + frame.local_108;
  frame.local_e8 = frame.local_11c * k_f_n0_25 + frame.local_110;
  frame.local_e4 = frame.local_118 * k_f_n0_25 + frame.local_10c;
  frame.local_e0 = frame.local_114 * k_f_n0_25 + frame.local_108;
  uVar2 = weapon_get_rw_801b14();
  iVar3 = FUN_004c4c30(uVar2,pfVar5,pfVar8);
  if ((iVar3 != 0) && (iVar3 = rocket_surface_hit_test(&frame.local_8c,&frame.local_110), iVar3 == 0)) {
    FUN_004c4720(&frame.local_104,&frame.local_f4,&frame.local_e8,(undefined4)frame.local_54);
    frame.local_104 = frame.local_8c * k_f_0_05 + frame.local_104;
    frame.local_100 = frame.local_88 * k_f_0_05 + frame.local_100;
    frame.local_fc = frame.local_84 * k_f_0_05 + frame.local_fc;
    FUN_004c51a0(&frame.local_40,(undefined4)frame.local_7c,&frame.local_104);
    fVar6 = (double)rp_random_float_range(0x3f333333,0x3f800000);
    frame.local_d8.y = (RwReal)fVar6;
    frame.local_d8.x = (RwReal)fVar6;
    frame.local_d8.z = 1.0;
    RwMatrixScale(&frame.local_40,&frame.local_d8,rwCOMBINEPRECONCAT);
    particle_pool_find_oldest_or_free(0,&frame.local_40,0);
    fVar6 = (double)rp_random_float_range(0x3ecccccd,0x3f19999a);
    fx_spawn_from_matrix_2(&frame.local_40,(fbits2 = (float)fVar6, *(int *)&fbits2));
  }
  frame.local_f8 = 0;
  pfVar5 = (float *)(param_1 + 8);
  do {
    out = (RwV3d *)(pfVar5 + -2);
    if (*(int *)&pfVar5[1] != 0) {
      iVar4 = (-(uint)(*(int *)(param_1 + 0xf8) != 0) & 10) + 0x50 + frame.local_f8;
      iVar3 = vehicle_part_frame(*(undefined4 *)(camera + 0x204),iVar4);
      if (iVar3 == 0) {
        return;
      }
      m = (RwMatrix *)vehicle_part_ltm(*(undefined4 *)(camera + 0x204),iVar4);
      RwFrameTransform(*(RwFrame **)(*(int *)&pfVar5[1] + 4),m,rwCOMBINEREPLACE);
      out->x = (m->pos).x - frame.local_9c;
      pfVar5[-1] = (m->pos).y - frame.local_98;
      *pfVar5 = (m->pos).z - frame.local_94;
      RwV3dNormalize(out,out);
      emitter = *(int *)&pfVar5[1];
      out->x = out->x * k_f_0_25;
      pfVar5[-1] = pfVar5[-1] * k_f_0_25;
      *pfVar5 = *pfVar5 * k_f_0_25;
      RwFrameTranslate(*(RwFrame **)(emitter + 4),out,rwCOMBINEPOSTCONCAT);
      rwp_world_current_activate_r(*(undefined4 *)&pfVar5[2]);
      FUN_0056b440(*(undefined4 *)(*(int *)&pfVar5[2] + 0x24),*(int *)&pfVar5[2],6);
      camera = get_viewport_camera(0);
      camera = *(int *)(camera + 4);
      frame.local_104 = *(float *)(camera + 0x30);
      frame.local_128.x = *(float *)(camera + 0x40) - frame.local_9c;
      frame.local_100 = *(float *)(camera + 0x34);
      frame.local_fc = *(float *)(camera + 0x38);
      frame.local_128.y = *(float *)(camera + 0x44) - frame.local_98;
      frame.local_128.z = *(float *)(camera + 0x48) - frame.local_94;
      RwV3dNormalize(&frame.local_128,&frame.local_128);
      uVar19 = 0x3e99999a;
      uVar18 = 0xbe99999a;
      frame.local_128.x = frame.local_104 * float_0_8 + frame.local_128.x;
      frame.local_128.z = frame.local_fc * float_0_8 + frame.local_128.z;
      fVar6 = (double)rp_random_float_range(0xbe99999a, 0x3e99999a);
      uVar17 = 0x3fc00000;
      uVar16 = 0x3f800000;
      frame.local_128.x = (RwReal)(fVar6 + (double)frame.local_128.x);
      fVar6 = (double)rp_random_float_range(0x3f800000, 0x3fc00000);
      uVar15 = 0x3e99999a;
      frame.local_128.y = (RwReal)fVar6;
      uVar14 = 0xbe99999a;
      fVar6 = (double)rp_random_float_range(0xbe99999a, 0x3e99999a);
      uVar13 = 0x43480000;
      frame.local_128.x = frame.local_128.x * k_f_40;
      uVar12 = 0x42c80000;
      frame.local_128.y = frame.local_128.y * k_f_40;
      frame.local_128.z = (RwReal)((fVar6 + (double)frame.local_128.z) * (double)k_f_40);
      frame.local_11c = *pfVar5 - pfVar5[-1] * k_f_0;
      frame.local_118 = out->x * k_f_0 - *pfVar5 * k_f_0;
      frame.local_114 = pfVar5[-1] * k_f_0 - out->x;
      fVar6 = (double)rp_random_float_range(0x42c80000, 0x43480000);
      uVar11 = 0x43480000;
      uVar10 = 0x42c80000;
      frame.local_11c = (float)(fVar6 * (double)frame.local_11c);
      fVar6 = (double)rp_random_float_range(0x42c80000, 0x43480000);
      uVar9 = 0x43480000;
      uVar2 = 0x42c80000;
      frame.local_118 = (float)(fVar6 * (double)frame.local_118);
      fVar6 = (double)rp_random_float_range(0x42c80000, 0x43480000);
      emitter = *(int *)&pfVar5[1];
      pfVar8 = &frame.local_11c;
      frame.local_114 = (float)(fVar6 * (double)frame.local_114);
      debris_apply_velocity_a(emitter,pfVar8);
      debris_apply_velocity_b
                (*(int *)&pfVar5[1],&frame.local_128,0,emitter,pfVar8,uVar2,uVar9,uVar10,uVar11,uVar12,uVar13,uVar14,
                 uVar15,uVar16,uVar17,uVar18,uVar19);
      camera = 4;
      do {
        fVar6 = (double)rp_random_float_range(0x3ecccccd,0x3f4ccccd);
        fx_ring_add_point(*(int *)(*(int *)&pfVar5[1] + 4) + 0x40,(fbits3 = (float)fVar6, *(int *)&fbits3));
        camera = camera + -1;
      } while (camera != 0);
      camera = 5;
      do {
        fVar6 = (double)rp_random_float_range(0x3ecccccd,0x3f4ccccd);
        fx_ring_add_point(&m->pos,(fbits4 = (float)fVar6, *(int *)&fbits4));
        camera = camera + -1;
      } while (camera != 0);
      pbVar1 = (byte *)((frame.local_f8 >> 3) + 0xf0 + param_1);
      *pbVar1 = *pbVar1 | '\x01' << ((byte)frame.local_f8 & 7);
      pfVar5[3] = 0.0;
      camera = frame.local_dc;
    }
    frame.local_f8 = frame.local_f8 + 1;
    pfVar5 = pfVar5 + 6;
  } while (frame.local_f8 < 10);
  *(undefined4 *)(param_1 + 0xf4) = 1;
  return;
}


