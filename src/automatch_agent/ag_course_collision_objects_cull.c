#include "mfl_tier2.h"
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*006f4b48*/ extern undefined4 g_course_6f4b48;
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern int g_course_6f6908;


/* course_collision_objects_cull */

/* Frustum tests every collision object against the camera and pushes vectors for visible ones. */

void course_collision_objects_cull(RwCamera *camera)

{
  float *cam_pos;
  RwFrustumTestResult test;
  int i;
  RwV3d *vec_buf;
  int *visible_flag;
  RwV3d *obj;
  
  cam_pos = &(RwFrameGetLTM(RwCamera_object_object_parent(camera))->pos).x;
  i = 0;
  if (0 < g_course_6f6908) {
    obj = (RwV3d *)&g_course_6f4b48;
    vec_buf = (RwV3d *)&g_course_6f60c8;
    visible_flag = &g_course_6f68c8;
    do {
      *visible_flag = 0;
      test = RwCameraFrustumTestSphere(camera,(RwSphere *)(obj + 0x1a));
      if (test != rwSPHEREOUTSIDE) {
        course_collision_object_push_vectors(obj,cam_pos,vec_buf,visible_flag);
      }
      i = i + 1;
      visible_flag = visible_flag + 1;
      vec_buf = (RwV3d *)&vec_buf[10].z;
      obj = (RwV3d *)&obj[0x1c].z;
    } while (i < g_course_6f6908);
  }
  return;
}
