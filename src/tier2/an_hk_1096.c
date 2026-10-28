#include "mfl_tier2.h"
/*00481fd0*/ extern float random_range();
/*004b2b48*/ extern int _ftol();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern char *RpPTankAtomicCreate();
/*00771ab0*/ extern struct RpAtomic *g_data_771ab0;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/* layout of *ptank_data in course_storm_cb0_sub_4aaca0, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct course_storm_cb0_sub_4aaca0_iVar3_layout {
    char pad_0x0[216];
    undefined4 f_0xd8;
    undefined4 f_0xdc;
    undefined4 f_0xe0;
    undefined4 f_0xe4;
} course_storm_cb0_sub_4aaca0_iVar3_layout;

/*007dc68c*/ extern char g_data_7dc68c[];
#define PTANK_PLUGIN_FLAGS_OFFSET (0x40) /* flag word inside the PTank atomic plugin data */
/* course_storm_cb0_sub_4aaca0 */

/* single caller course_storm_cb0 */

/* Creates the storm particle-tank atomic g_data_771ab0 (0x200 particles, type 0x80007), sets its flags/bounding sphere (radius 1000), optional texture on material 0, blend 5/6 (inferred), then randomises per-particle colour bytes in g_data_7dc68c (stride 0x20). */
void course_storm_cb0_sub_4aaca0(RwTexture *particle_texture)

{
  uint *flags_ptr;
  RpMorphTarget *morph_target;
  course_storm_cb0_sub_4aaca0_iVar3_layout *ptank_data;
  undefined1 colour_byte;
  RwFrame *frame;
  undefined2 *particle_cursor;
  
  frame = RwFrameCreate();
  g_data_771ab0 = (RpAtomic *)RpPTankAtomicCreate(0x200,0x80007,0);
  flags_ptr = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + PTANK_PLUGIN_FLAGS_OFFSET);
  *flags_ptr = *flags_ptr | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 4) =
       0x200;
  RpAtomicSetFrame(g_data_771ab0,frame);
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(g_data_771ab0));
  RpMorphTarget_boundingSphere(morph_target).center.x = 0.0;
  RpMorphTarget_boundingSphere(morph_target).center.y = 0.0;
  RpMorphTarget_boundingSphere(morph_target).center.z = 0.0;
  RpMorphTarget_boundingSphere(morph_target).radius = 1000.0;
  ptank_data = (course_storm_cb0_sub_4aaca0_iVar3_layout *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset));
  ptank_data->f_0xd8 = 0x3f800000;
  ptank_data->f_0xdc = 0x3f800000;
  ptank_data->f_0xe0 = 0;
  ptank_data->f_0xe4 = 0;
  flags_ptr = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + PTANK_PLUGIN_FLAGS_OFFSET);
  *flags_ptr = *flags_ptr | 0x80000;
  if (particle_texture != (RwTexture *)0x0) {
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_data_771ab0)),particle_texture);
  }
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xac) =
       1;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xa4) =
       5;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xa8) =
       6;
  flags_ptr = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + PTANK_PLUGIN_FLAGS_OFFSET);
  *flags_ptr = *flags_ptr | 0x10000000;
  particle_cursor = (undefined2 *)((char *)&g_data_7dc68c + 0x56);
  do {
    particle_cursor[-1] = 0;
    *particle_cursor = 0;
    colour_byte = (int)random_range(0x43380000,0x43600000);
    *(undefined1 *)(particle_cursor + 0xd) = colour_byte;
    colour_byte = (int)random_range(0x433e0000,0x43660000);
    *(undefined1 *)((char *)particle_cursor + 0x1b) = colour_byte;
    colour_byte = (int)random_range(0x43420000,0x436a0000);
    *(undefined1 *)(particle_cursor + 0xe) = colour_byte;
    *(undefined1 *)((char *)particle_cursor + 0x1d) = 0x80;
    particle_cursor = particle_cursor + 0x10;
  } while ((char *)particle_cursor < (char *)&g_ptank_plugin_offset + 0x6026);
  return;
}


