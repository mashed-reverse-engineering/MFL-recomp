#include "mfl_tier2.h"
/*00406950*/ extern undefined __cdecl world_for_all_materials_cb_406910(struct RpWorld *p0, void *p1);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00451d10*/ extern int camera_create_a();
/*00451dc0*/ extern int camera_create_b();
/*00475730*/ extern int get_viewport_camera();
/*00495b60*/ extern undefined __cdecl viewport_raster_create(uint p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00495c10*/ extern int viewport_get_field_702254();
/*0049a9b0*/ extern int fx_slot_find_free();
/*004a0e40*/ extern int rain_splash_add();
/*004a0f20*/ extern int rain_ptank_create();
/*004a1860*/ extern int game_set_option_flags();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(undefined4 p0);
/*005dd7e8*/ extern float k_f_60;
/*006a0770*/ extern undefined4 g_course_arctic_6a0770;
/*006a0774*/ extern undefined4 g_course_arctic_6a0774;
/*006a0778*/ extern int g_course_arctic_6a0778;
/*008c5730*/ extern float g_course_arctic_8c5730;
/*008c5734*/ extern float g_course_arctic_8c5734;
/*008c5738*/ extern float g_course_arctic_8c5738;
/*008c573c*/ extern float g_course_arctic_8c573c;
/*008c5740*/ extern undefined4 g_course_arctic_8c5740;
/*008c5840*/ extern RpClump *g_course_arctic_8c5840;
/*008c5844*/ extern undefined4 g_course_arctic_8c5844[0x1000];
/* course_arctic_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_arctic_cb0(int param_1)

{
  void *texture_dict;
  RpAtomic *atomic;
  int camera_2;
  undefined4 uVar1;
  RwCamera *camera;
  int iVar2;
  int *piVar3;
  int iVar4;
  RwReal farClip;
  RwV3d RStack_24;
  struct {
    undefined4 uStack_18;
    undefined4 uStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  g_course_arctic_6a0770 =
       RwTexDictionaryFindNamedTexture((struct RwTexDictionary *)(*(RwTexDictionary **)(param_1 + 0xc9d0)),(RwChar *)("lightning"));
  viewport_raster_create(0,0x80,0x80,0x600);
  texture_dict = (void *)viewport_get_field_702254(0);
  world_for_all_materials_cb_406910(*(RpWorld **)(param_1 + 0xc9d4),texture_dict);
  camera_create_a(param_1);
  camera_create_b(param_1);
  RwFrameSetIdentity(*(RwFrame **)(*(int *)(param_1 + 0xc118) + 4));
  g_course_arctic_8c5840 = *(RpClump **)(param_1 + 0xc118);
  RStack_24.x = 0.0;
  RStack_24.y = -4.1;
  RStack_24.z = 0.0;
  atomic = (RpAtomic *)clump_first_atomic(g_course_arctic_8c5840);
  if ((RpAtomic_interpolator_flags(atomic) & 2) != 0) {
    _rpAtomicResyncInterpolatedSphere(atomic);
  }
  g_course_arctic_8c5730 = RpAtomic_boundingSphere(atomic).center.x;
  g_course_arctic_8c5734 = RpAtomic_boundingSphere(atomic).center.y;
  g_course_arctic_8c5738 = RpAtomic_boundingSphere(atomic).center.z;
  g_course_arctic_8c573c = RpAtomic_boundingSphere(atomic).radius;
  RpWorldRemoveClump(*(RpWorld **)(param_1 + 0xc9d4),g_course_arctic_8c5840);
  RwFrameSetIdentity(RpClump_object_parent(g_course_arctic_8c5840));
  piVar3 = g_course_arctic_8c5844;
  do {
    camera_2 = FUN_004f6bf0((undefined4)g_course_arctic_8c5840);
    *piVar3 = camera_2;
    RwFrameSetIdentity(*(RwFrame **)(camera_2 + 4));
    piVar3 = piVar3 + 1;
  } while ((int)piVar3 < (int)g_course_arctic_8c5844 + 0x60);
  RStack_24.x = -150.0;
  iVar4 = 0;
  camera_2 = 5;
  do {
    RStack_24.z = -150.0;
    iVar2 = 5;
    do {
      RwFrameTranslate(*(RwFrame **)((int)(&g_course_arctic_8c5840)[iVar4] + 4),&RStack_24,
                       rwCOMBINEREPLACE);
      RStack_24.z = RStack_24.z + k_f_60;
      iVar4 = iVar4 + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    camera_2 = camera_2 + -1;
    RStack_24.x = RStack_24.x + k_f_60;
  } while (camera_2 != 0);
  frame.uStack_c = 0xc23af5c3;
  frame.uStack_8 = 0x3f3f3b64;
  frame.uStack_4 = 0xc28c245a;
  frame.uStack_18 = 0xbc03126f;
  frame.uStack_14 = 0x3ca3d70a;
  frame.uStack_10 = 0xbc03126f;
  RStack_24.x = 0.008;
  RStack_24.y = 0.08;
  RStack_24.z = 0.008;
  uVar1 = find_texture_dict_b("smoke");
  rain_ptank_create(uVar1);
  rain_splash_add(&frame.uStack_c,0x3c23d70a,&frame.uStack_18,&RStack_24);
  camera_2 = 0;
  if (0 < *(int *)(param_1 + 0xc004)) {
    iVar4 = param_1 + 0xc30;
    do {
      uVar1 = fx_slot_find_free(0xc,iVar4,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = uVar1;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      camera_2 = camera_2 + 1;
      iVar4 = iVar4 + 0x40;
    } while (camera_2 < *(int *)(param_1 + 0xc004));
  }
  camera_2 = 0;
  if (0 < *(int *)(param_1 + 0xc008)) {
    iVar4 = param_1 + 0x1830;
    do {
      uVar1 = fx_slot_find_free(7,iVar4,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = uVar1;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      camera_2 = camera_2 + 1;
      iVar4 = iVar4 + 0x40;
    } while (camera_2 < *(int *)(param_1 + 0xc008));
  }
  camera_2 = 0;
  if (0 < *(int *)(param_1 + 0xc00c)) {
    iVar4 = param_1 + 0x2430;
    do {
      uVar1 = fx_slot_find_free(2,iVar4,0,0x14);
      (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = uVar1;
      g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
      camera_2 = camera_2 + 1;
      iVar4 = iVar4 + 0x40;
    } while (camera_2 < *(int *)(param_1 + 0xc00c));
  }
  frame.uStack_c = 0xc204199a;
  frame.uStack_8 = 0x401147ae;
  frame.uStack_4 = 0xc2286666;
  g_course_arctic_6a0774 = g_course_arctic_6a0778;
  uVar1 = fx_slot_find_free(0xc,&frame.uStack_c,0,0x14);
  (&g_course_arctic_8c5740)[g_course_arctic_6a0778] = uVar1;
  g_course_arctic_6a0778 = g_course_arctic_6a0778 + 1;
  if (*(float *)(param_1 + 0xc9f8) == *(float *)(param_1 + 0xc9fc)) {
    farClip = 800.0;
    camera = (RwCamera *)get_viewport_camera(0);
    RwCameraSetFarClipPlane(camera,farClip);
    camera_2 = get_viewport_camera(0);
    *(undefined4 *)(camera_2 + 0x88) = 0x43480000;
    get_viewport_camera(0);
    game_set_option_flags(0x28,0x2c,0x30);
  }
  return;
}


