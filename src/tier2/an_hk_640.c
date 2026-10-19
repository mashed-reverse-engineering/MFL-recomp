#include "mfl_tier2.h"
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dca20*/ extern RwBool __cdecl RwIm3DRenderIndexedPrimitive(enum RwPrimitiveType p0, RwImVertexIndex *p1, RwInt32 p2);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define BOX_CORNER_COUNT (8)
#define BOX_LINE_INDEX_COUNT (24)
#define IM3D_TRANSFORM_FLAGS (rwIM3D_ALLOPAQUE)
#define SHADE_MODE_FLAT (rwSHADEMODEFLAT)
#define SHADE_MODE_GOURAUD (rwSHADEMODEGOURAUD)
/* debug_box_draw_with_culling_a */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Original note: "draws the 12 edges of an AABB (24 indices) with colour from param_3, transformed by the frame
   LTM"
   Draws the 12 edges of an axis-aligned box as a flat-shaded line list, in the colour given by
   `colour` (R,G,B bytes; alpha forced to 0xFF), transformed by the LTM of the frame at clump+4.
   `box`: [0..2] = min x,y,z and [3..5] = max x,y,z (inferred).
   `clump`: object whose frame pointer is at +4 (inferred). Returns nothing. The lines are only
   submitted when RwIm3DTransform succeeds (inferred: it returns NULL when the box is culled). */

void debug_box_draw_with_culling_a(char *clump,undefined4 *box,byte *colour)

{
  RwMatrix *frame_ltm;
  void *transformed;
  /* 12 line segments as index pairs, indexing the 8 corners. Corner bit 0 = min x,
     bit 1 = min y, bit 2 = min z (inferred from the vertex fill below). */
  RwImVertexIndex edge_indices [BOX_LINE_INDEX_COUNT];
  /* first of BOX_CORNER_COUNT corner vertices (stride 0x24), filled below; the fields are
     written through local_xx slots */
  RwIm3DVertex corners;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  uint local_e8;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  uint local_c4;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_b0;
  uint local_a0;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  uint local_7c;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  uint local_58;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  uint local_34;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_10;

  /* edge list: pairs of corner indices */
  edge_indices[5] = 2;
  edge_indices[6] = 2;
  edge_indices[0xd] = 6;
  edge_indices[0xe] = 6;
  edge_indices[0x15] = 6;
  edge_indices[9] = 5;
  edge_indices[10] = 5;
  edge_indices[0x13] = 5;
  edge_indices[0] = 0;
  edge_indices[1] = 1;
  edge_indices[2] = 1;
  edge_indices[3] = 3;
  edge_indices[4] = 3;
  edge_indices[7] = 0;
  edge_indices[8] = 4;
  edge_indices[0xb] = 7;
  edge_indices[0xc] = 7;
  edge_indices[0xf] = 4;
  edge_indices[0x10] = 0;
  edge_indices[0x11] = 4;
  edge_indices[0x12] = 1;
  edge_indices[0x14] = 2;
  edge_indices[0x16] = 3;
  edge_indices[0x17] = 7;
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(clump + 4));
  /* colour: bytes R,G,B packed as 0xFFRRGGBB (the 0xffffff00 mask forces alpha opaque) */
  corners.color = ((*colour | 0xffffff00) << 8 | (uint)colour[1]) << 8 | (uint)colour[2];
  /* corner 0 = max corner (box[3..5]); corners 1..7 follow the bit order above */
  corners.objVertex.x = *(float *)&box[3];
  corners.objVertex.y = *(float *)&box[4];
  corners.objVertex.z = *(float *)&box[5];
  local_100 = *box;
  local_fc = box[4];
  local_f8 = box[5];
  local_dc = box[3];
  local_d8 = box[1];
  local_d4 = box[5];
  local_b8 = *box;
  local_b4 = box[1];
  local_b0 = box[5];
  local_94 = box[3];
  local_90 = box[4];
  local_8c = box[2];
  local_70 = *box;
  local_6c = box[4];
  local_68 = box[2];
  local_4c = box[3];
  local_48 = box[1];
  local_44 = box[2];
  local_28 = *box;
  local_24 = box[1];
  local_20 = box[2];
  local_e8 = corners.color;
  local_c4 = corners.color;
  local_a0 = corners.color;
  local_7c = corners.color;
  local_58 = corners.color;
  local_34 = corners.color;
  local_10 = corners.color;
  /* draw in flat shade mode, then restore Gouraud */
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)SHADE_MODE_FLAT);
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  transformed = RwIm3DTransform(&corners,BOX_CORNER_COUNT,frame_ltm,IM3D_TRANSFORM_FLAGS);
  /* only submit the line list if the transform produced output */
  if (transformed != (void *)0x0) {
    RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST,edge_indices,BOX_LINE_INDEX_COUNT);
    RwIm3DEnd();
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)SHADE_MODE_GOURAUD);
  return;
}


