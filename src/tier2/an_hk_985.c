#include "mfl_tier2.h"
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_quad_render_b */

/* "same quad draw for the second quad set (+0xE8 points, texture at +0x118)" */

void __cdecl course_quad_render_b(int param_1,undefined4 param_2,undefined4 param_3)

{
  int corner;
  void *transformed;
  struct {
    float tex[8];      /* v coordinates 0..3, then u coordinates 4..7 */
    RwV3d corner[4];      /* the quad points transformed by the object matrix */
    RwIm3DVertex vertex[4];
  } frame;
  
  frame.tex[4] = 0.0;
  frame.tex[5] = 1.0;
  frame.tex[6] = 0.0;
  frame.tex[7] = 1.0;
  frame.tex[0] = 1.0;
  frame.tex[1] = 1.0;
  frame.tex[2] = 0.0;
  frame.tex[3] = 0.0;
  RwV3dTransformPoints(frame.corner,(RwV3d *)(param_1 + 0xe8),4,(RwMatrix *)(param_1 + 0xa8));
  for (corner = 0; corner < 4; corner = corner + 1) {
    frame.vertex[corner].objVertex = frame.corner[corner];
    frame.vertex[corner].u = frame.tex[4 + corner];
    frame.vertex[corner].v = frame.tex[corner];
    frame.vertex[corner].color = 0x60ffffff;
  }
  frame.vertex[0].color = 0xffffff;
  frame.vertex[1].color = 0xffffff;
  if (*(undefined4 **)(param_1 + 0x118) == (undefined4 *)0x0) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  }
  else {
    RwRenderStateSet
              (rwRENDERSTATETEXTURERASTER,(void *)**(undefined4 **)(param_1 + 0x118));
  }
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  transformed = RwIm3DTransform(frame.vertex,4,(RwMatrix *)0x0,1);
  if (transformed != (void *)0x0) {
    RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
    RwIm3DEnd();
    return;
  }
  return;
}
