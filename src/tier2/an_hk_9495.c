#include "mfl_tier2.h"
extern char g_sky_offsets[];
extern char g_sky_drift[];
/*00481fd0*/ extern double random_range();
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*004b2b48*/ extern int _ftol();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern char *RpPTankAtomicCreate();
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

/* "creates the frame / atomic of the sky object (g_course_6a06e0)" */

void course_sky_atomic_create(RwTexture *texture,int size_hi,undefined4 size_lo)

{
  uint *ptank_flags;
  RwV3d *vec_ptr;
  RpMorphTarget *morph_target;
  undefined1 uVar4;
  RwFrame *frame;
  int byte_off;
  uint particle_index;
  int next_off;
  double rnd;
  RwV2d dir2d;
  struct {
    undefined1 *puStack_30;
    int iStack_2c;
    undefined4 uStack_28;
    undefined4 uStack_24;
    RwReal sphere_x;
    RwReal sphere_y;
    RwReal sphere_z;
    RwReal sphere_radius;
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
  fo.sphere_x = 0.0;
  fo.sphere_y = 0.0;
  fo.sphere_z = 0.0;
  fo.sphere_radius = 1000.0f;
  g_sky_atomic = (RpAtomic *)RpPTankAtomicCreate(0x100,0x10080003,0);
  g_course_sky_6a06e4 = size_hi;
  g_course_sky_6a06e8 = size_lo;
  g_course_sky_8c7eb0 = (undefined4)texture;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 4) = 0x100
  ;
  RpAtomicSetFrame(g_sky_atomic,frame);
  byte_off = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
  *(undefined4 *)(byte_off + 0xb8) = fo.uStack_28;
  *(undefined4 *)(byte_off + 0xbc) = fo.uStack_24;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x4000;
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_sky_atomic));
  RpMorphTarget_boundingSphere(morph_target).center.x = fo.sphere_x;
  RpMorphTarget_boundingSphere(morph_target).center.y = fo.sphere_y;
  RpMorphTarget_boundingSphere(morph_target).center.z = fo.sphere_z;
  RpMorphTarget_boundingSphere(morph_target).radius = fo.sphere_radius;
  particle_index = 0;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xac) = 1;
  byte_off = 0;
  do {
    rnd = (double)random_range(g_course_sky_6a06e8,g_course_sky_6a06e4);
    *(float *)(byte_off + ((char *)&g_data_69d6dc + 0x8)) = (float)rnd;
    *(uint *)(byte_off + ((char *)&g_data_69d6dc + 0x10)) = particle_index & 0xff;
    rnd = (double)random_range(0xbf800000,0x3f800000);
    dir2d.x = (RwReal)rnd;
    rnd = (double)random_range(0xbf800000,0x3f800000);
    dir2d.y = (RwReal)rnd;
    RwV2dNormalize(&dir2d,&dir2d);
    rnd = (double)random_range(0x3dcccccd,0x3f800000);
    vec_ptr = (RwV3d *)(byte_off + ((char *)g_sky_offsets + 0x0));
    dir2d.x = (RwReal)((double)dir2d.x * rnd);
    dir2d.y = (RwReal)((double)dir2d.y * rnd);
    rnd = (double)random_range(0xbf800000,0x3f800000);
    vec_ptr->x = (float)rnd;
    rnd = (double)random_range(0xbf800000,0x3f800000);
    *(float *)(byte_off + ((char *)g_sky_offsets + 0x8)) = (float)rnd;
    rnd = (double)random_range(0xbf800000,0x3f800000);
    *(float *)(byte_off + ((char *)g_sky_offsets + 0x4)) = (float)rnd;
    RwV3dNormalize(vec_ptr,vec_ptr);
    rnd = (double)random_range(0x3dcccccd,0x3f800000);
    vec_ptr->x = (float)(rnd * (double)vec_ptr->x);
    rnd = (double)random_range(0x3dcccccd,0x3f800000);
    *(float *)(byte_off + ((char *)g_sky_offsets + 0x4)) = (float)(rnd * (double)*(float *)(byte_off + ((char *)g_sky_offsets + 0x4)));
    rnd = (double)random_range(0x3dcccccd,0x3f800000);
    *(float *)(byte_off + ((char *)g_sky_offsets + 0x8)) = (float)(rnd * (double)*(float *)(byte_off + ((char *)g_sky_offsets + 0x8)));
    vec_ptr = (RwV3d *)(byte_off + ((char *)g_sky_drift + 0x0));
    rnd = (double)random_range(0xbdcccccd,0x3dcccccd);
    vec_ptr->x = (float)rnd;
    rnd = (double)random_range(0xbf800000,0);
    *(float *)(byte_off + ((char *)g_sky_drift + 0x4)) = (float)rnd;
    rnd = (double)random_range(0xbdcccccd,0x3dcccccd);
    *(float *)(byte_off + ((char *)g_sky_drift + 0x8)) = (float)rnd;
    RwV3dNormalize(vec_ptr,vec_ptr);
    rnd = (double)random_range(0x3f000000,0x3f800000);
    vec_ptr->x = (float)(rnd * (double)vec_ptr->x);
    rnd = (double)random_range(0x3f000000,0x3f800000);
    *(float *)(byte_off + ((char *)g_sky_drift + 0x4)) = (float)(rnd * (double)*(float *)(byte_off + ((char *)g_sky_drift + 0x4)));
    rnd = (double)random_range(0x3f000000,0x3f800000);
    next_off = byte_off + 0x10;
    *(float *)(byte_off + ((char *)g_sky_drift + 0x8)) = (float)(rnd * (double)*(float *)(byte_off + ((char *)g_sky_drift + 0x8)));
    particle_index = particle_index + 1;
    byte_off = next_off;
  } while (next_off < 0x1000);
  if (texture != (RwTexture *)0x0) {
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_sky_atomic)),texture);
    byte_off = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
    *(undefined4 *)(byte_off + 0xd8) = fo.uStack_10;
    *(undefined4 *)(byte_off + 0xdc) = fo.uStack_c;
    *(undefined4 *)(byte_off + 0xe0) = fo.uStack_8;
    *(undefined4 *)(byte_off + 0xe4) = fo.uStack_4;
    byte_off = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
    *(uint *)(byte_off + 0x40) = *(uint *)(byte_off + 0x40) | 0x80000;
  }
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xa4) = 5;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset) + 0xa8) = 6;
  byte_off = *(int *)(&RpAtomic_object_object_type(g_sky_atomic) + g_ptank_plugin_offset);
  *(uint *)(byte_off + 0x40) = *(uint *)(byte_off + 0x40) | 0x10000000;
  RpPTankAtomicLock_00545d10(g_sky_atomic,&fo.puStack_30,2,0x40000000);
  byte_off = 0x100;
  do {
    *fo.puStack_30 = 0x20;
    fo.puStack_30[1] = 0x20;
    fo.puStack_30[2] = 0x40;
    fo.puStack_30[3] = 0xff;
    fo.puStack_30 = fo.puStack_30 + fo.iStack_2c;
    byte_off = byte_off + -1;
  } while (byte_off != 0);
  size_hi = 0;
  do {
    cos(((double)size_hi - (double)k_f_128) * (double)k_f_0_024543693);
    uVar4 = ((int)((cos((((float)size_hi - 128.0f) * 0.024543693f)) + 1.0f) * 127.5f));
    *(undefined1 *)(size_hi + ((char *)&g_course_sky_8c7eb0 + 0x10)) = uVar4;
    size_hi = size_hi + 1;
  } while (size_hi < 0x100);
  return;
}


