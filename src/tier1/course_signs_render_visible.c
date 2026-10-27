#include "mfl_tier2.h"
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8864*/ extern undefined4 g_course_8c8864;
/*008c8ae4*/ extern char g_game_8c8ae4[];

#define SIGN_ENTRY_STRIDE 0x14 /* ints per sign entry (80 bytes) */
#define SIGN_LIST_END ((int)&g_game_8c8ae4) /* end address of the sign entry list */


/* course_signs_render_visible */

/* "frustum tests each atomic and renders its three parts" */

/* Renders each enabled sign entry whose bounding sphere is inside the camera frustum. */
void course_signs_render_visible(void)
{
  struct { RwSphere * sphere; RwFrustumTestResult frustum_result; int * sign; } lcl_;
#define sphere lcl_.sphere
#define frustum_result lcl_.frustum_result
#define sign lcl_.sign
  
  sign = &g_course_8c8864;
  do {
    /* sign[0x10] = enabled flag; sign[-1] = atomic; sign[0..2] = the three parts */
    if ((sign[0x10] != 0) && (sign[-1] != 0)) {
      sphere = RpAtomicGetWorldBoundingSphere((RpAtomic *)*sign);
      frustum_result = RwCameraFrustumTestSphere((RwCamera *)RwCameraGetCurrentCamera(),sphere);
      if (frustum_result != rwSPHEREOUTSIDE) {
        /* vtable slot 0x48 of each part (name unknown) */
        (**(code **)(*sign + 0x48))(*sign);
        (**(code **)(sign[2] + 0x48))(sign[2]);
        (**(code **)(sign[1] + 0x48))(sign[1]);
      }
    }
    sign = sign + SIGN_ENTRY_STRIDE;
  } while ((int)sign < SIGN_LIST_END);
  return;
}
