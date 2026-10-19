#include "mfl_tier2.h"
/*0049ac10*/ extern undefined __cdecl fx_ptank_atomic_b_create(struct RwTexture *p0);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern void * RpPTankAtomicCreate();
/*00759698*/ extern struct RpAtomic *g_fx_ptank_atomic_a;
/*007596a0*/ extern undefined4 g_particles_7596a0;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;

#define PTANK_PARTICLE_COUNT (0x200)
#define PTANK_PLUGIN_COUNT_OFFSET (0x4) /* (inferred) particle count field in the ptank plugin data */
#define PTANK_PLUGIN_FLAGS_OFFSET (0x40) /* (inferred) flags field in the ptank plugin data */

/* fx_ptank_atomics_create */

/* "creates the 0x200 particle ptank atomic a once and then atomic b" */

/*
 * Creates the ptank particle atomic g_fx_ptank_atomic_a (PTANK_PARTICLE_COUNT particles)
 * the first time it is called, then creates atomic b through fx_ptank_atomic_b_create.
 * Later calls do nothing. Parameter `texture` is the particle texture; it may be NULL,
 * in which case no texture is set on the material. Returns nothing.
 */
void fx_ptank_atomics_create(RwTexture *texture)

{
  uint *plugin_flags;
  RpMorphTarget *morph_target;
  RwFrame *frame;

  frame = RwFrameCreate();
  /* one-time setup: g_particles_7596a0 is set to 1 at the end */
  if (g_particles_7596a0 == 0) {
    g_fx_ptank_atomic_a = (RpAtomic *)RpPTankAtomicCreate(PTANK_PARTICLE_COUNT,0x10000087,0);
    *(undefined4 *)
     (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) + 0xa4) = 5;
    *(undefined4 *)
     (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) + 0xa8) = 2;
    plugin_flags = (uint *)(*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) +
                     PTANK_PLUGIN_FLAGS_OFFSET);
    *plugin_flags = *plugin_flags | 0x10000000;
    plugin_flags = (uint *)(*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) +
                     PTANK_PLUGIN_FLAGS_OFFSET);
    *plugin_flags = *plugin_flags | 0x800000;
    *(undefined4 *)
     (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) + PTANK_PLUGIN_COUNT_OFFSET) = PTANK_PARTICLE_COUNT;
    RpAtomicSetFrame(g_fx_ptank_atomic_a,frame);
    /* bounding sphere of the morph target: centred on the origin, radius 10 (inferred) */
    morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_fx_ptank_atomic_a));
    (RpMorphTarget_boundingSphere(morph_target)).center.x = 0.0;
    (RpMorphTarget_boundingSphere(morph_target)).center.y = 0.0;
    (RpMorphTarget_boundingSphere(morph_target)).center.z = 0.0;
    (RpMorphTarget_boundingSphere(morph_target)).radius = 10.0;
    /* only when a texture was passed: set its filter/addressing mode and apply it to the material */
    if (texture != (RwTexture *)0x0) {
      *(undefined1 *)&RwTexture_filterAddressing(texture) = 2;
      RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_fx_ptank_atomic_a)),texture);
    }
    *(undefined4 *)
     (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_a) + g_ptank_plugin_offset) + 0xac) = 1;
    g_particles_7596a0 = 1;
    fx_ptank_atomic_b_create(texture);
  }
  return;
}


