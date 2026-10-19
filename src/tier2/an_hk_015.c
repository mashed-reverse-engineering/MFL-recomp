#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern void * RpPTankAtomicCreate();
/*0075969c*/ extern struct RpAtomic *g_fx_ptank_atomic_b;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;

#define PTANK_PARTICLE_COUNT (0x2000) /* particles in the ptank atomic b (inferred from the comment below) */
#define D3DBLEND_SRCALPHA (5) /* ptank plugin src blend (inferred from the "blend 5/2" comment) */
#define D3DBLEND_ONE (2) /* ptank plugin dst blend (inferred from the "blend 5/2" comment) */

/* fx_ptank_atomic_b_create */

/* "creates the 0x2000 particle ptank atomic b (blend 5/2) with its texture" */

/* Creates the global particle-tank atomic g_fx_ptank_atomic_b (SRCALPHA/ONE blend, 10-unit bounding sphere at the origin).
 * texture: texture for its material; may be NULL, in which case the material keeps no texture.
 * Returns nothing; the result is stored in g_fx_ptank_atomic_b. (inferred) */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void __cdecl fx_ptank_atomic_b_create(RwTexture *texture)
{
  uint *plugin_flags;
  RpMorphTarget *morph_target;
  RwFrame *frame;

  /* new frame for the atomic, then create the atomic itself */
  frame = RwFrameCreate();
  g_fx_ptank_atomic_b = (RpAtomic *)RpPTankAtomicCreate(PTANK_PARTICLE_COUNT,0x10000087,0);
  /* plugin data: src and dst blend factors */
  *(undefined4 *)
   (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) + 0xa4) = D3DBLEND_SRCALPHA;
  *(undefined4 *)
   (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) + 0xa8) = D3DBLEND_ONE;
  /* set two bits in the plugin flag word at +0x40 (meanings unknown) */
  plugin_flags = (uint *)(*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) +
                   0x40);
  *plugin_flags = *plugin_flags | 0x10000000;
  plugin_flags = (uint *)(*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) +
                   0x40);
  *plugin_flags = *plugin_flags | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) + 4)
       = 0x200;
  /* attach the frame, then set the morph target's bounding sphere: centre at origin, radius 10 */
  RpAtomicSetFrame(g_fx_ptank_atomic_b,frame);
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_fx_ptank_atomic_b));
  (RpMorphTarget_boundingSphere(morph_target)).center.x = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.y = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.z = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).radius = 10.0;
  /* optional texture: set its addressing byte to 2 (clamp, inferred), then apply it to the first material */
  if (texture != (RwTexture *)0x0) {
    *(undefined1 *)&RwTexture_filterAddressing(texture) = 2;
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_fx_ptank_atomic_b)),texture);
  }
  /* last plugin field write (+0xac) */
  *(undefined4 *)
   (*(int *)(&RpAtomic_object_object_type(g_fx_ptank_atomic_b) + g_ptank_plugin_offset) + 0xac) = 1;
  return;
}
