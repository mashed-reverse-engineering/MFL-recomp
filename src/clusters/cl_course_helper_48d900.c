// cluster-entries: course_helper_48d900
/* Installed cluster course_helper_48d900 (tools/cluster_install.py from output/cluster_pilot_md/course_helper_48d900.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern struct RpAtomic * __cdecl RpPTankAtomicCreate(int count, int flags, int p2);
/*00545d10*/ extern void __cdecl RpPTankAtomicLock_00545d10(struct RpAtomic *atomic, void *locked, int flags, int mode);
/*00545f20*/ extern void __cdecl RpPTankAtomicUnlock_00545f20(struct RpAtomic *atomic);
/*006f6930*/ extern int g_rw_6f6930;
/*006f693c*/ extern struct RpAtomic *g_ptank_atomic;
/*006f6940*/ extern struct RwTexture *g_data_6f6940;
/*006f6944*/ extern int g_ptank_lock_6f6944;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern int g_ptank_plugin_offset;
/*0088a844*/ extern void *g_rw_88a844;
/* course_helper_48d900 */

#define PTANK_PARTICLE_COUNT 0x800

/* the locked colour buffer of a PTank atomic: write pointer and stride */
typedef struct PTankLock {
  unsigned char *data;
  int stride;
} PTankLock;

/* the PTank plugin data of an atomic (flags at +0x40, particle size at +0xb8 / +0xbc ...) */
#define PTANK_DATA(atomic) (*(unsigned char **)((char *)(atomic) + g_ptank_plugin_offset))

/* "callers course_neustein_cb0, course_warzone_cb0: creates the particle tank of the course (2048 white particles of size 0.1 with a bounding
   sphere of radius 1000 at the origin) and binds the given texture to it" */
void course_helper_48d900(struct RwTexture *texture)
{
  RwFrame *frame;
  PTankLock lock;
  RpMorphTarget *morph;
  float size[2];
  float sphere[4];
  int i;

  frame = RwFrameCreate();
  size[0] = 0.1f;
  size[1] = 0.1f;
  g_rw_88a844 = RwMalloc(0x1000, 0);
  sphere[0] = 0.0f;
  sphere[1] = 0.0f;
  sphere[2] = 0.0f;
  sphere[3] = 1000.0f;
  g_data_6f6940 = texture;
  g_ptank_atomic = RpPTankAtomicCreate(PTANK_PARTICLE_COUNT, 0x87, 0);
  *(int *)(PTANK_DATA(g_ptank_atomic) + 0xa4) = 5;
  *(int *)(PTANK_DATA(g_ptank_atomic) + 0xa8) = 6;
  *(unsigned int *)(PTANK_DATA(g_ptank_atomic) + 0x40) |= 0x10000000;
  *(unsigned int *)(PTANK_DATA(g_ptank_atomic) + 0x40) |= 0x800000;
  *(int *)(PTANK_DATA(g_ptank_atomic) + 4) = 0;
  RpAtomicSetFrame(g_ptank_atomic, frame);
  morph = RpGeometry_morphTarget(RpAtomic_geometry(g_ptank_atomic));
  RpMorphTarget_boundingSphere(morph).center.x = sphere[0];
  RpMorphTarget_boundingSphere(morph).center.y = sphere[1];
  RpMorphTarget_boundingSphere(morph).center.z = sphere[2];
  RpMorphTarget_boundingSphere(morph).radius = sphere[3];
  *(int *)(PTANK_DATA(g_ptank_atomic) + 0xac) = 1;
  *(float *)(PTANK_DATA(g_ptank_atomic) + 0xb8) = size[0];
  *(float *)(PTANK_DATA(g_ptank_atomic) + 0xbc) = size[1];
  *(unsigned int *)(PTANK_DATA(g_ptank_atomic) + 0x40) |= 0x4000;
  if (g_data_6f6940 != 0) {
    *(unsigned char *)((char *)g_data_6f6940 + 0x50) = 1;
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_ptank_atomic)), g_data_6f6940);
  }
  RpPTankAtomicLock_00545d10(g_ptank_atomic, &lock, 2, 0x40000000);
  for (i = 0; i < PTANK_PARTICLE_COUNT; i = i + 1) {
    lock.data[0] = 0xff;
    lock.data[1] = 0xff;
    lock.data[2] = 0xff;
    lock.data[3] = 0xff;
    lock.data = lock.data + lock.stride;
  }
  RpPTankAtomicUnlock_00545f20(g_ptank_atomic);
  g_rw_6f6930 = 0;
  g_ptank_lock_6f6944 = 0;
}

