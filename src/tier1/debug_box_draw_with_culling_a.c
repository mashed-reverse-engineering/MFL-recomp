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
  RwIm3DVertex v[BOX_CORNER_COUNT];

  edge_indices[0] = 0;
  edge_indices[1] = 1;
  edge_indices[2] = 1;
  edge_indices[3] = 3;
  edge_indices[4] = 3;
  edge_indices[5] = 2;
  edge_indices[6] = 2;
  edge_indices[7] = 0;
  edge_indices[8] = 4;
  edge_indices[9] = 5;
  edge_indices[10] = 5;
  edge_indices[11] = 7;
  edge_indices[12] = 7;
  edge_indices[13] = 6;
  edge_indices[14] = 6;
  edge_indices[15] = 4;
  edge_indices[16] = 0;
  edge_indices[17] = 4;
  edge_indices[18] = 1;
  edge_indices[19] = 5;
  edge_indices[20] = 2;
  edge_indices[21] = 6;
  edge_indices[22] = 3;
  edge_indices[23] = 7;
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(clump + 4));
  /* colour: bytes R,G,B packed as 0xFFRRGGBB (the 0xffffff00 mask forces alpha opaque) */
  v[0].color = ((*colour | 0xffffff00) << 8 | (uint)colour[1]) << 8 | (uint)colour[2];
  v[0].objVertex.x = ((float *)box)[3];
  v[0].objVertex.y = ((float *)box)[4];
  v[0].objVertex.z = ((float *)box)[5];
  v[1].objVertex.x = ((float *)box)[0];
  v[1].objVertex.y = ((float *)box)[4];
  v[1].objVertex.z = ((float *)box)[5];
  v[1].color = v[0].color;
  v[2].objVertex.x = ((float *)box)[3];
  v[2].objVertex.y = ((float *)box)[1];
  v[2].objVertex.z = ((float *)box)[5];
  v[2].color = v[0].color;
  v[3].objVertex.x = ((float *)box)[0];
  v[3].objVertex.y = ((float *)box)[1];
  v[3].objVertex.z = ((float *)box)[5];
  v[3].color = v[0].color;
  v[4].objVertex.x = ((float *)box)[3];
  v[4].objVertex.y = ((float *)box)[4];
  v[4].objVertex.z = ((float *)box)[2];
  v[4].color = v[0].color;
  v[5].objVertex.x = ((float *)box)[0];
  v[5].objVertex.y = ((float *)box)[4];
  v[5].objVertex.z = ((float *)box)[2];
  v[5].color = v[0].color;
  v[6].objVertex.x = ((float *)box)[3];
  v[6].objVertex.y = ((float *)box)[1];
  v[6].objVertex.z = ((float *)box)[2];
  v[6].color = v[0].color;
  v[7].objVertex.x = ((float *)box)[0];
  v[7].objVertex.y = ((float *)box)[1];
  v[7].objVertex.z = ((float *)box)[2];
  v[7].color = v[0].color;
  /* draw in flat shade mode, then restore Gouraud */
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)SHADE_MODE_FLAT);
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  transformed = RwIm3DTransform(v,BOX_CORNER_COUNT,frame_ltm,IM3D_TRANSFORM_FLAGS);
  /* only submit the line list if the transform produced output */
  if (transformed != (void *)0x0) {
    RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST,edge_indices,BOX_LINE_INDEX_COUNT);
    RwIm3DEnd();
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)SHADE_MODE_GOURAUD);
  return;
}
