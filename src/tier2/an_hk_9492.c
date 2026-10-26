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

/* "PTank atomic with tex for the rain" */

void rain_ptank_create(RwTexture *rain_texture)

{
  uint *flags;
  RpMorphTarget *morph_target;
  RwTexture *tex;
  RwFrame *frame;
  RpAtomic *ptank_atomic;
  int counter_or_data;
  /* one contiguous frame: the lock record, the two uv scale words and the bounding sphere are read through the pointer to lock_cursor */
  struct {
    undefined1 *lock_cursor;
    int lock_stride;
    undefined4 uv_scale_x;
    undefined4 uv_scale_y;
    RwReal sphere_x;
    RwReal sphere_y;
    RwReal sphere_z;
    RwReal sphere_radius;
    undefined4 uv_u0;
    undefined4 uv_v0;
    undefined4 uv_u1;
    undefined4 uv_v1;
  } frame_obj;
  
  frame = RwFrameCreate();
  frame_obj.uv_u0 = 0;
  frame_obj.uv_v0 = 0;
  frame_obj.uv_u1 = 0x3f800000;
  frame_obj.uv_v1 = 0x3f800000;
  frame_obj.uv_scale_x = 0x3dcccccd;
  frame_obj.uv_scale_y = 0x3dcccccd;
  g_app_885820 = (undefined4)(RwMalloc(0x4000,0));
  frame_obj.sphere_x = 0.0;
  frame_obj.sphere_y = 0.0;
  frame_obj.sphere_z = 0.0;
  frame_obj.sphere_radius = 1000.0;
  g_rain_ptank_76e5b0 = rain_texture;
  ptank_atomic = (RpAtomic *)RpPTankAtomicCreate(0x200,0x80007,0);
  *(undefined4 *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0xa4) = 5;
  *(undefined4 *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0xa8) = 2;
  flags = (uint *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0x40);
  *flags = *flags | 0x10000000;
  flags = (uint *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0x40);
  *flags = *flags | 0x800000;
  g_app_76e5ac = ptank_atomic;
  *(undefined4 *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 4) = 0x200;
  RpAtomicSetFrame(ptank_atomic,frame);
  ptank_atomic = g_app_76e5ac;
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_app_76e5ac));
  RpMorphTarget_boundingSphere(morph_target).center.x = frame_obj.sphere_x;
  RpMorphTarget_boundingSphere(morph_target).center.y = frame_obj.sphere_y;
  RpMorphTarget_boundingSphere(morph_target).center.z = frame_obj.sphere_z;
  RpMorphTarget_boundingSphere(morph_target).radius = frame_obj.sphere_radius;
  *(undefined4 *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0xac) = 1;
  counter_or_data = *(int *)((char *)ptank_atomic + g_ptank_plugin_offset);
  *(undefined4 *)(counter_or_data + 0xd8) = frame_obj.uv_u0;
  *(undefined4 *)(counter_or_data + 0xdc) = frame_obj.uv_v0;
  *(undefined4 *)(counter_or_data + 0xe0) = frame_obj.uv_u1;
  *(undefined4 *)(counter_or_data + 0xe4) = frame_obj.uv_v1;
  counter_or_data = *(int *)((char *)ptank_atomic + g_ptank_plugin_offset);
  *(uint *)(counter_or_data + 0x40) = *(uint *)(counter_or_data + 0x40) | 0x80000;
  counter_or_data = *(int *)((char *)ptank_atomic + g_ptank_plugin_offset);
  *(undefined4 *)(counter_or_data + 0xb8) = frame_obj.uv_scale_x;
  *(undefined4 *)(counter_or_data + 0xbc) = frame_obj.uv_scale_y;
  flags = (uint *)(*(char **)((char *)ptank_atomic + g_ptank_plugin_offset) + 0x40);
  *flags = *flags | 0x4000;
  tex = g_rain_ptank_76e5b0;
  if (g_rain_ptank_76e5b0 != (RwTexture *)0x0) {
    *(undefined1 *)&RwTexture_filterAddressing(g_rain_ptank_76e5b0) = 2;
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(ptank_atomic)),tex);
    ptank_atomic = g_app_76e5ac;
  }
  RpPTankAtomicLock_00545d10(ptank_atomic,&frame_obj.lock_cursor,2,0x40000000);
  counter_or_data = 0x200;
  do {
    *frame_obj.lock_cursor = 0xff;
    frame_obj.lock_cursor[1] = 0xff;
    frame_obj.lock_cursor[2] = 0xff;
    frame_obj.lock_cursor[3] = 0xff;
    frame_obj.lock_cursor = frame_obj.lock_cursor + frame_obj.lock_stride;
    counter_or_data = counter_or_data + -1;
  } while (counter_or_data != 0);
  counter_or_data = 0;
  do {
    *(undefined4 *)(counter_or_data + (int)g_app_885820) = 0;
    *(undefined4 *)(counter_or_data + 4 + (int)g_app_885820) = 0;
    counter_or_data = counter_or_data + 0x20;
  } while (counter_or_data < 0x4000);
  g_app_76e5a0 = 0;
  rain_slots_clear();
  return;
}


