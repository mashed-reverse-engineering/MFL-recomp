#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1e80*/ extern void FUN_004d1e80(undefined4 p0);
/*006c2944*/ extern undefined4 g_car_6c2944;
/*006c2948*/ extern undefined4 g_aux_camera_6c2948;
/* aux_camera_destroy */
/*
 * Tears down the auxiliary camera. Takes no parameters and returns nothing.
 *  1. Detaches the frame from g_car_6c2944 (the car object).
 *  2. Destroys the RwFrame held in g_aux_camera_6c2948 (the aux camera).
 *  3. Calls FUN_004d1e80 on g_car_6c2944 (inferred: finishes the car's
 *     per-object cleanup; the name is still a placeholder).
 */
void aux_camera_destroy(void)
{
  _rwObjectHasFrameSetFrame(g_car_6c2944,(RwFrame *)0x0);
  RwFrameDestroy(g_aux_camera_6c2948);
  FUN_004d1e80(g_car_6c2944);
  return;
}
