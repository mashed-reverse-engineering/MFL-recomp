#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*00481fd0*/ extern float random_range();
/*004b2b48*/ extern int _ftol();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*006a0770*/ extern undefined4 *g_course_arctic_6a0770;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c590c*/ extern undefined4 g_course_arctic_8c590c;
/* course_arctic_snow_quad_draw */

/* "draws a textured quad of the snow object at a random alpha, 4 corners relative to the object" */

void course_arctic_snow_quad_draw(char *param_1,char *param_2)

{
  int u_row;
  int v_row;
  int alpha;
  int corner;
  void *transformed;
  RwCamera *camera;
  RwFrustumTestResult test_result;
  RwSphere *sphere;
  struct {
    RwV3d corner[4];      /* quad corners relative to the object */
    float u_table[8];      /* two rows of 4 texture u coordinates (the first row doubles as the culling sphere afterwards) */
    float v_table[8];      /* two rows of 4 texture v coordinates */
    RwIm3DVertex vertex[4];
  } frame;
  
  frame.corner[0].x = -0.5;
  frame.corner[0].y = 4.0;
  frame.corner[0].z = 5.0;
  frame.corner[1].x = 0.5;
  frame.corner[1].y = 4.0;
  frame.corner[1].z = 5.0;
  frame.corner[2].x = -0.5;
  frame.corner[2].y = -2.0;
  frame.corner[2].z = 5.0;
  frame.corner[3].x = 0.5;
  frame.corner[3].y = -2.0;
  frame.corner[3].z = 5.0;
  frame.u_table[0] = 1.0;
  frame.u_table[1] = 0.0;
  frame.u_table[2] = 1.0;
  frame.u_table[3] = 0.0;
  frame.u_table[4] = 0.0;
  frame.u_table[5] = 1.0;
  frame.u_table[6] = 0.0;
  frame.u_table[7] = 1.0;
  frame.v_table[0] = 0.0;
  frame.v_table[1] = 0.0;
  frame.v_table[2] = 1.0;
  frame.v_table[3] = 1.0;
  frame.v_table[4] = 1.0;
  frame.v_table[5] = 1.0;
  frame.v_table[6] = 0.0;
  frame.v_table[7] = 0.0;
  if (g_course_arctic_6a0770 != (undefined4 *)0x0) {
    RwRenderStateSet
              (rwRENDERSTATETEXTURERASTER,(void *)*g_course_arctic_6a0770);
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x2);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwV3dTransformPoints(frame.corner,frame.corner,4,(RwMatrix *)(param_1 + 4));
  alpha = (int)random_range(0,0x437f0000);
  u_row = *(int *)(param_1 + 0x44);
  v_row = *(int *)(param_1 + 0x48);
  for (corner = 0; corner < 4; corner = corner + 1) {
    frame.vertex[corner].objVertex.x = frame.corner[corner].x + *(float *)(param_2 + 0x30);
    frame.vertex[corner].objVertex.y = frame.corner[corner].y + *(float *)(param_2 + 0x34);
    frame.vertex[corner].objVertex.z = frame.corner[corner].z + *(float *)(param_2 + 0x38);
    frame.vertex[corner].color = alpha << 0x18 | 0xffffff;
    frame.vertex[corner].u = frame.u_table[u_row * 4 + corner];
    frame.vertex[corner].v = frame.v_table[v_row * 4 + corner];
  }
  transformed = RwIm3DTransform(frame.vertex,4,(RwMatrix *)0x0,1);
  if (transformed != (void *)0x0) {
    RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
    RwIm3DEnd();
  }
  sphere = (RwSphere *)frame.u_table;
  sphere->center.x = *(float *)(param_1 + 0x34) + *(float *)(param_2 + 0x30);
  sphere->center.y = *(float *)(param_1 + 0x38) + *(float *)(param_2 + 0x34);
  sphere->radius = 0.25;
  sphere->center.z = *(float *)(param_1 + 0x3c) + *(float *)(param_2 + 0x38);
  camera = (RwCamera *)get_viewport_camera(0);
  test_result = RwCameraFrustumTestSphere(camera,sphere);
  if ((test_result != rwSPHEREINSIDE) && (test_result != rwSPHEREBOUNDARY)) {
    g_course_arctic_8c590c = 0;
    return;
  }
  g_course_arctic_8c590c = (int)(alpha + (alpha >> 0x1f & 7U)) >> 3;
  return;
}
