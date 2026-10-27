#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00497880*/ extern int fx_pool_create();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern void * RpPTankAtomicCreate();
/*0072dfd8*/ extern undefined4 g_particles_72dfd8;
/*00745bd8*/ extern undefined4 g_particles_745bd8;
/*00745bdc*/ extern undefined4 g_particles_745bdc;
/*00745be0*/ extern undefined4 g_particles_745be0;
/*00745be4*/ extern undefined4 g_particles_745be4;
/*00745be8*/ extern undefined4 g_fx_spritecomp_745be8;
/*00745bec*/ extern undefined4 g_fx_spritecomp_745bec;
/*00745bf0*/ extern undefined4 g_particles_745bf0;
/*00745bf4*/ extern undefined4 g_fx_spritecomp_745bf4;
/*00745bf8*/ extern undefined4 g_fx_spritecomp_745bf8;
/*00745bfc*/ extern undefined4 g_fx_spritecomp_745bfc;
/*00745c00*/ extern undefined4 g_fx_spritecomp_745c00;
/*00745c04*/ extern undefined4 g_fx_spritecomp_745c04;
/*00745c08*/ extern undefined4 g_fx_spritecomp_745c08;
/*00745c0c*/ extern undefined4 g_fx_spritecomp_745c0c;
/*00745c10*/ extern undefined4 g_fx_spritecomp_745c10;
/*00745c14*/ extern undefined4 g_fx_spritecomp_745c14;
/*00745c18*/ extern undefined4 g_fx_spritecomp_745c18;
/*00745c1c*/ extern undefined4 g_fx_spritecomp_745c1c;
/*00745c20*/ extern undefined4 g_fx_spritecomp_745c20;
/*00745c24*/ extern undefined4 g_fx_spritecomp_745c24;
/*00745c28*/ extern undefined4 g_fx_spritecomp_745c28;
/*00745c2c*/ extern undefined4 g_fx_spritecomp_745c2c;
/*00745c30*/ extern undefined4 g_fx_spritecomp_745c30;
/*00745c34*/ extern undefined4 g_fx_spritecomp_745c34;
/*00745c38*/ extern undefined4 g_fx_spritecomp_745c38;
/*00745c3c*/ extern undefined4 g_fx_spritecomp_745c3c;
/*00745c40*/ extern undefined4 g_fx_spritecomp_745c40;
/*00745c44*/ extern undefined4 g_fx_spritecomp_745c44;
/*007519a4*/ extern struct RpAtomic *g_fx_atomic_a;
/*007519a8*/ extern struct RpAtomic *g_fx_atomic_b;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;

#define SPRITE_PARAM_SETS (7)
#define PARTICLE_CLEAR_WORDS (0x5f00)

/* fx_spritecomp_create */

/* "'spriteComp' PTank atomic and texture" */

/*
 * Builds the two PTank particle atomics for the 'spriteComp' sprite effect
 * (g_fx_atomic_a, g_fx_atomic_b). Takes no parameters, returns nothing.
 * Steps (inferred from the callees): fill the sprite parameter table, create
 * both atomics, set each one's PTank plugin fields and flag bits, give each a
 * frame and a 1000-unit bounding sphere at the origin, assign the 'spriteComp'
 * texture to both materials, zero the particle buffer, then call fx_pool_create.
 */
void fx_spritecomp_create(void)

{
  uint *ptank_flags;
  RpMorphTarget *morph_target;
  RpAtomic *atomic;
  int scratch;
  RwFrame *frame;
  RwTexture *texture;
  undefined4 *clear_cursor;

  /* Fill the SPRITE_PARAM_SETS parameter sets: case N writes 4 words of
     g_particles_745bd8 .. g_fx_spritecomp_745c44 (values are float bit patterns). */
  scratch = 0;
  do {
    switch(scratch) {
    case 0:
      g_particles_745bd8 = 0;
      g_particles_745bdc = 0;
      g_particles_745be0 = 0x3e800000;
      g_particles_745be4 = 0x3f000000;
      break;
    case 1:
      g_fx_spritecomp_745be8 = 0x3f600000;
      g_fx_spritecomp_745bec = 0x3e800000;
      g_particles_745bf0 = 0x3f800000;
      g_fx_spritecomp_745bf4 = 0x3f000000;
      break;
    case 2:
      g_fx_spritecomp_745bf8 = 0x3e800000;
      g_fx_spritecomp_745bfc = 0;
      g_fx_spritecomp_745c00 = 0x3f000000;
      g_fx_spritecomp_745c04 = 0x3f000000;
      break;
    case 3:
      g_fx_spritecomp_745c08 = 0x3e800000;
      g_fx_spritecomp_745c0c = 0x3f000000;
      g_fx_spritecomp_745c10 = 0x3f000000;
      g_fx_spritecomp_745c14 = 0x3f800000;
      break;
    case 4:
      g_fx_spritecomp_745c18 = 0;
      g_fx_spritecomp_745c1c = 0x3f000000;
      g_fx_spritecomp_745c20 = 0x3e800000;
      g_fx_spritecomp_745c24 = 0x3f800000;
      break;
    case 5:
      g_fx_spritecomp_745c28 = 0x3f000000;
      g_fx_spritecomp_745c2c = 0;
      g_fx_spritecomp_745c30 = 0x3f600000;
      g_fx_spritecomp_745c34 = 0x3f200000;
      break;
    case 6:
      g_fx_spritecomp_745c38 = 0x3f600000;
      g_fx_spritecomp_745c3c = 0;
      g_fx_spritecomp_745c40 = 0x3f800000;
      g_fx_spritecomp_745c44 = 0x3e800000;
    }
    scratch = scratch + 1;
  } while (scratch < SPRITE_PARAM_SETS);
  g_fx_atomic_a = (RpAtomic *)RpPTankAtomicCreate(0x500,0x20087,0);
  g_fx_atomic_b = (RpAtomic *)RpPTankAtomicCreate(0x500,0x20087,0);
  /* PTank plugin setup for both atomics: plugin fields at +0x4, +0xa4, +0xa8,
     +0xac, +0xc0 and bit flags OR'd into the plugin flags word at +0x40
     (field meanings unknown, not named). */
  atomic = g_fx_atomic_a;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_a) + g_ptank_plugin_offset) + 0xa4) = 5
  ;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xa8) = 6;
  scratch = *(int *)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset);
  *(uint *)(scratch + 0x40) = *(uint *)(scratch + 0x40) | 0x10000000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0xa4) = 5
  ;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0xa8) = 2
  ;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x10000000;
  scratch = *(int *)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset);
  *(uint *)(scratch + 0x40) = *(uint *)(scratch + 0x40) | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 4) = 0;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 4) = 0;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xc0) = 0;
  scratch = *(int *)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset);
  *(uint *)(scratch + 0x40) = *(uint *)(scratch + 0x40) | 0x20000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0xc0) = 0
  ;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x20000;
  /* Give each atomic its own frame. */
  frame = RwFrameCreate();
  RpAtomicSetFrame(g_fx_atomic_a,frame);
  frame = RwFrameCreate();
  RpAtomicSetFrame(g_fx_atomic_b,frame);
  /* Bounding sphere of each morph target: centre (0,0,0), radius 1000. */
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_fx_atomic_a));
  (RpMorphTarget_boundingSphere(morph_target)).center.x = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.y = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.z = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).radius = 1000.0;
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_fx_atomic_b));
  (RpMorphTarget_boundingSphere(morph_target)).center.x = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.y = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.z = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).radius = 1000.0;
  /* Look up the 'spriteComp' texture and set it on each atomic's first material. */
  texture = (RwTexture *)find_texture_dict_b("spriteComp");
  RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_fx_atomic_a)),texture);
  atomic = g_fx_atomic_b;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_a) + g_ptank_plugin_offset) + 0xac) = 1
  ;
  RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(atomic)),texture);
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_fx_atomic_b) + g_ptank_plugin_offset) + 0xac) = 1
  ;
  /* Zero PARTICLE_CLEAR_WORDS words of the particle buffer starting at g_particles_72dfd8. */
  clear_cursor = &g_particles_72dfd8;
  for (scratch = PARTICLE_CLEAR_WORDS; scratch != 0; scratch = scratch + -1) {
    *clear_cursor = 0;
    clear_cursor = clear_cursor + 1;
  }
  fx_pool_create();
  return;
}


