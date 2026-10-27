#include "mfl_tier2.h"
/*00481fd0*/ extern float random_range();
/*004b2b48*/ extern int _ftol();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern char *RpPTankAtomicCreate();
/*00771ab0*/ extern struct RpAtomic *g_data_771ab0;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/* layout of *iVar3 in course_storm_cb0_sub_4aaca0, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct course_storm_cb0_sub_4aaca0_iVar3_layout {
    char pad_0x0[216];
    undefined4 f_0xd8;
    undefined4 f_0xdc;
    undefined4 f_0xe0;
    undefined4 f_0xe4;
} course_storm_cb0_sub_4aaca0_iVar3_layout;

/*007dc68c*/ extern char g_data_7dc68c[];
/* course_storm_cb0_sub_4aaca0 */

/* single caller course_storm_cb0 */

void course_storm_cb0_sub_4aaca0(RwTexture *param_1)

{
  uint *puVar1;
  RpMorphTarget *pRVar2;
  course_storm_cb0_sub_4aaca0_iVar3_layout *iVar3;
  undefined1 uVar4;
  RwFrame *frame;
  undefined2 *puVar5;
  
  frame = RwFrameCreate();
  g_data_771ab0 = (RpAtomic *)RpPTankAtomicCreate(0x200,0x80007,0);
  puVar1 = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x800000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 4) =
       0x200;
  RpAtomicSetFrame(g_data_771ab0,frame);
  pRVar2 = RpGeometry_morphTarget(RpAtomic_geometry(g_data_771ab0));
  RpMorphTarget_boundingSphere(pRVar2).center.x = 0.0;
  RpMorphTarget_boundingSphere(pRVar2).center.y = 0.0;
  RpMorphTarget_boundingSphere(pRVar2).center.z = 0.0;
  RpMorphTarget_boundingSphere(pRVar2).radius = 1000.0;
  iVar3 = (course_storm_cb0_sub_4aaca0_iVar3_layout *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset));
  iVar3->f_0xd8 = 0x3f800000;
  iVar3->f_0xdc = 0x3f800000;
  iVar3->f_0xe0 = 0;
  iVar3->f_0xe4 = 0;
  puVar1 = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x80000;
  if (param_1 != (RwTexture *)0x0) {
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(g_data_771ab0)),param_1);
  }
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xac) =
       1;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xa4) =
       5;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0xa8) =
       6;
  puVar1 = (uint *)(*(char **)(&RpAtomic_object_object_type(g_data_771ab0) + g_ptank_plugin_offset) + 0x40);
  *puVar1 = *puVar1 | 0x10000000;
  puVar5 = (undefined2 *)((char *)&g_data_7dc68c + 0x56);
  do {
    puVar5[-1] = 0;
    *puVar5 = 0;
    uVar4 = (int)random_range(0x43380000,0x43600000);
    *(undefined1 *)(puVar5 + 0xd) = uVar4;
    uVar4 = (int)random_range(0x433e0000,0x43660000);
    *(undefined1 *)((char *)puVar5 + 0x1b) = uVar4;
    uVar4 = (int)random_range(0x43420000,0x436a0000);
    *(undefined1 *)(puVar5 + 0xe) = uVar4;
    *(undefined1 *)((char *)puVar5 + 0x1d) = 0x80;
    puVar5 = puVar5 + 0x10;
  } while ((char *)puVar5 < (char *)&g_ptank_plugin_offset + 0x6026);
  return;
}


