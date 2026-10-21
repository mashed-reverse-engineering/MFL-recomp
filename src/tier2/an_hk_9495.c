#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*004b2b48*/ extern int _ftol();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern int RpPTankAtomicCreate();
/*005ddb28*/ extern float k_f_128;
/*005df2b0*/ extern float k_f_0_024543693;
/*006a06e0*/ extern struct RpAtomic *g_sky_atomic;
/*006a06e4*/ extern undefined4 g_course_sky_6a06e4;
/*006a06e8*/ extern undefined4 g_course_sky_6a06e8;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*008c7eb0*/ extern undefined4 g_course_sky_8c7eb0;
extern double cos(double);
extern char g_data_69d6dc[];
/* course_sky_atomic_create */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "creates the frame / atomic of the sky object (g_course_6a06e0)" */

void course_sky_atomic_create(RwTexture *texture,int param_2,undefined4 param_3)

{
  uint *puVar1;
  RwV3d *pRVar2;
  RpMorphTarget *pRVar3;
  undefined1 uVar4;
  RwFrame *frame;
  int iVar5;
  uint uVar6;
  int iVar7;
  float10 fVar8;
  RwV2d RStack_38;
  struct {
    undefined1 *puStack_30;
    int iStack_2c;
    undefined4 uStack_28;
    undefined4 uStack_24;
    RwReal RStack_20;
    RwReal RStack_1c;
    RwReal RStack_18;
    RwReal RStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } fo;
  
  fo.uStack_28 = 0x3d4ccccd;
  fo.uStack_24 = 0x3f000000;
  frame = RwFrameCreate();
  fo.uStack_10 = 0x3f800000;
  fo.uStack_c = 0x3f800000;
  fo.uStack_8 = 0;
  fo.uStack_4 = 0;
  fo.RStack_20 = 0.0;
  fo.RStack_1c = 0.0;
  fo.RStack_18 = 0.0;
  fo.RStack_14 = 1000.0f;
  g_sky_atomic = (RpAtomic *)RpPTankAtomicCreate(0x100,0x10080003,0);
  g_course_sky_6a06e4 = param_2;
  g_course_sky_6a06e8 = param_3;
  g_course_sky_8c7eb0 = (undefined4)texture;
  puVar1 = (uint *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x800000;
  *(undefined4 *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 4) = 0x100
  ;
  RpAtomicSetFrame(g_sky_atomic,frame);
  iVar5 = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
  *(undefined4 *)(iVar5 + 0xb8) = fo.uStack_28;
  *(undefined4 *)(iVar5 + 0xbc) = fo.uStack_24;
  puVar1 = (uint *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x4000;
  pRVar3 = RpGeometry_morphTarget(RpAtomic_geometry(g_sky_atomic));
  RpMorphTarget_boundingSphere(pRVar3).center.x = fo.RStack_20;
  RpMorphTarget_boundingSphere(pRVar3).center.y = fo.RStack_1c;
  RpMorphTarget_boundingSphere(pRVar3).center.z = fo.RStack_18;
  RpMorphTarget_boundingSphere(pRVar3).radius = fo.RStack_14;
  uVar6 = 0;
  *(undefined4 *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xac) = 1;
  iVar5 = 0;
  do {
    fVar8 = (float10)random_range(g_course_sky_6a06e8,g_course_sky_6a06e4);
    *(float *)(iVar5 + ((int)&g_data_69d6dc + 0x8)) = (float)fVar8;
    *(uint *)(iVar5 + ((int)&g_data_69d6dc + 0x10)) = uVar6 & 0xff;
    fVar8 = (float10)random_range(0xbf800000,0x3f800000);
    RStack_38.x = (RwReal)fVar8;
    fVar8 = (float10)random_range(0xbf800000,0x3f800000);
    RStack_38.y = (RwReal)fVar8;
    RwV2dNormalize(&RStack_38,&RStack_38);
    fVar8 = (float10)random_range(0x3dcccccd,0x3f800000);
    pRVar2 = (RwV3d *)(iVar5 + 0x69e6e0);
    RStack_38.x = (RwReal)((float10)RStack_38.x * fVar8);
    RStack_38.y = (RwReal)((float10)RStack_38.y * fVar8);
    fVar8 = (float10)random_range(0xbf800000,0x3f800000);
    pRVar2->x = (float)fVar8;
    fVar8 = (float10)random_range(0xbf800000,0x3f800000);
    *(float *)(iVar5 + 0x69e6e8) = (float)fVar8;
    fVar8 = (float10)random_range(0xbf800000,0x3f800000);
    *(float *)(iVar5 + 0x69e6e4) = (float)fVar8;
    RwV3dNormalize(pRVar2,pRVar2);
    fVar8 = (float10)random_range(0x3dcccccd,0x3f800000);
    pRVar2->x = (float)(fVar8 * (float10)pRVar2->x);
    fVar8 = (float10)random_range(0x3dcccccd,0x3f800000);
    *(float *)(iVar5 + 0x69e6e4) = (float)(fVar8 * (float10)*(float *)(iVar5 + 0x69e6e4));
    fVar8 = (float10)random_range(0x3dcccccd,0x3f800000);
    *(float *)(iVar5 + 0x69e6e8) = (float)(fVar8 * (float10)*(float *)(iVar5 + 0x69e6e8));
    pRVar2 = (RwV3d *)(iVar5 + 0x69f6e0);
    fVar8 = (float10)random_range(0xbdcccccd,0x3dcccccd);
    pRVar2->x = (float)fVar8;
    fVar8 = (float10)random_range(0xbf800000,0);
    *(float *)(iVar5 + 0x69f6e4) = (float)fVar8;
    fVar8 = (float10)random_range(0xbdcccccd,0x3dcccccd);
    *(float *)(iVar5 + 0x69f6e8) = (float)fVar8;
    RwV3dNormalize(pRVar2,pRVar2);
    fVar8 = (float10)random_range(0x3f000000,0x3f800000);
    pRVar2->x = (float)(fVar8 * (float10)pRVar2->x);
    fVar8 = (float10)random_range(0x3f000000,0x3f800000);
    *(float *)(iVar5 + 0x69f6e4) = (float)(fVar8 * (float10)*(float *)(iVar5 + 0x69f6e4));
    fVar8 = (float10)random_range(0x3f000000,0x3f800000);
    iVar7 = iVar5 + 0x10;
    *(float *)(iVar5 + 0x69f6e8) = (float)(fVar8 * (float10)*(float *)(iVar5 + 0x69f6e8));
    uVar6 = uVar6 + 1;
    iVar5 = iVar7;
  } while (iVar7 < 0x1000);
  if (texture != (RwTexture *)0x0) {
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_sky_atomic)),texture);
    iVar5 = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
    *(undefined4 *)(iVar5 + 0xd8) = fo.uStack_10;
    *(undefined4 *)(iVar5 + 0xdc) = fo.uStack_c;
    *(undefined4 *)(iVar5 + 0xe0) = fo.uStack_8;
    *(undefined4 *)(iVar5 + 0xe4) = fo.uStack_4;
    iVar5 = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
    *(uint *)(iVar5 + 0x40) = *(uint *)(iVar5 + 0x40) | 0x80000;
  }
  *(undefined4 *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xa4) = 5;
  *(undefined4 *)(*(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xa8) = 6;
  iVar5 = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
  *(uint *)(iVar5 + 0x40) = *(uint *)(iVar5 + 0x40) | 0x10000000;
  RpPTankAtomicLock_00545d10(g_sky_atomic,&fo.puStack_30,2,0x40000000);
  iVar5 = 0x100;
  do {
    *fo.puStack_30 = 0x20;
    fo.puStack_30[1] = 0x20;
    fo.puStack_30[2] = 0x40;
    fo.puStack_30[3] = 0xff;
    fo.puStack_30 = fo.puStack_30 + fo.iStack_2c;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  param_2 = 0;
  do {
    cos(((float10)param_2 - (float10)k_f_128) * (float10)k_f_0_024543693);
    uVar4 = ((int)((cos((((float)param_2 - 128.0f) * 0.024543693f)) + 1.0f) * 127.5f));
    *(undefined1 *)(param_2 + ((int)&g_course_sky_8c7eb0 + 0x10)) = uVar4;
    param_2 = param_2 + 1;
  } while (param_2 < 0x100);
  return;
}


