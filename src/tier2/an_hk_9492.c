#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*004a0ef0*/ extern int rain_slots_clear();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern char *RpPTankAtomicCreate();
/*0076e5a0*/ extern undefined4 g_app_76e5a0;
/*0076e5ac*/ extern RpAtomic *g_app_76e5ac;
/*0076e5b0*/ extern RwTexture *g_rain_ptank_76e5b0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*00885820*/ extern undefined4 g_app_885820;
/* rain_ptank_create */

/* "PTank atomic with texture for the rain" */

void rain_ptank_create(RwTexture *param_1)

{
  uint *puVar1;
  RpMorphTarget *pRVar2;
  RwTexture *texture;
  RwFrame *frame;
  RpAtomic *pRVar3;
  int iVar4;
  /* one contiguous frame: the lock record, the two uv scale words and the bounding sphere are read through the pointer to puStack_30 */
  struct {
    undefined1 *puStack_30;
    int iStack_2c;
    undefined4 local_28;
    undefined4 local_24;
    RwReal RStack_20;
    RwReal RStack_1c;
    RwReal RStack_18;
    RwReal RStack_14;
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame_obj;
  
  frame = RwFrameCreate();
  frame_obj.local_10 = 0;
  frame_obj.local_c = 0;
  frame_obj.local_8 = 0x3f800000;
  frame_obj.local_4 = 0x3f800000;
  frame_obj.local_28 = 0x3dcccccd;
  frame_obj.local_24 = 0x3dcccccd;
  g_app_885820 = (undefined4)(RwMalloc(0x4000,0));
  frame_obj.RStack_20 = 0.0;
  frame_obj.RStack_1c = 0.0;
  frame_obj.RStack_18 = 0.0;
  frame_obj.RStack_14 = 1000.0;
  g_rain_ptank_76e5b0 = param_1;
  pRVar3 = (RpAtomic *)RpPTankAtomicCreate(0x200,0x80007,0);
  *(undefined4 *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0xa4) = 5;
  *(undefined4 *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0xa8) = 2;
  puVar1 = (uint *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x10000000;
  puVar1 = (uint *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x800000;
  g_app_76e5ac = pRVar3;
  *(undefined4 *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 4) = 0x200;
  RpAtomicSetFrame(pRVar3,frame);
  pRVar3 = g_app_76e5ac;
  pRVar2 = RpGeometry_morphTarget(RpAtomic_geometry(g_app_76e5ac));
  RpMorphTarget_boundingSphere(pRVar2).center.x = frame_obj.RStack_20;
  RpMorphTarget_boundingSphere(pRVar2).center.y = frame_obj.RStack_1c;
  RpMorphTarget_boundingSphere(pRVar2).center.z = frame_obj.RStack_18;
  RpMorphTarget_boundingSphere(pRVar2).radius = frame_obj.RStack_14;
  *(undefined4 *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0xac) = 1;
  iVar4 = *(int *)((char *)pRVar3 + g_ptank_plugin_offset);
  *(undefined4 *)(iVar4 + 0xd8) = frame_obj.local_10;
  *(undefined4 *)(iVar4 + 0xdc) = frame_obj.local_c;
  *(undefined4 *)(iVar4 + 0xe0) = frame_obj.local_8;
  *(undefined4 *)(iVar4 + 0xe4) = frame_obj.local_4;
  iVar4 = *(int *)((char *)pRVar3 + g_ptank_plugin_offset);
  *(uint *)(iVar4 + 0x40) = *(uint *)(iVar4 + 0x40) | 0x80000;
  iVar4 = *(int *)((char *)pRVar3 + g_ptank_plugin_offset);
  *(undefined4 *)(iVar4 + 0xb8) = frame_obj.local_28;
  *(undefined4 *)(iVar4 + 0xbc) = frame_obj.local_24;
  puVar1 = (uint *)(*(char **)((char *)pRVar3 + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x4000;
  texture = g_rain_ptank_76e5b0;
  if (g_rain_ptank_76e5b0 != (RwTexture *)0x0) {
    *(undefined1 *)&RwTexture_filterAddressing(g_rain_ptank_76e5b0) = 2;
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(pRVar3)),texture);
    pRVar3 = g_app_76e5ac;
  }
  RpPTankAtomicLock_00545d10(pRVar3,&frame_obj.puStack_30,2,0x40000000);
  iVar4 = 0x200;
  do {
    *frame_obj.puStack_30 = 0xff;
    frame_obj.puStack_30[1] = 0xff;
    frame_obj.puStack_30[2] = 0xff;
    frame_obj.puStack_30[3] = 0xff;
    frame_obj.puStack_30 = frame_obj.puStack_30 + frame_obj.iStack_2c;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  iVar4 = 0;
  do {
    *(undefined4 *)(iVar4 + (int)g_app_885820) = 0;
    *(undefined4 *)(iVar4 + 4 + (int)g_app_885820) = 0;
    iVar4 = iVar4 + 0x20;
  } while (iVar4 < 0x4000);
  g_app_76e5a0 = 0;
  rain_slots_clear();
  return;
}


