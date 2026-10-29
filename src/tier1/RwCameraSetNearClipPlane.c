/* compile: /G5 */
#include "mfl_tier2.h"
/*004d1c00*/ extern void __cdecl CameraUpdateZShiftScale(void *cam);
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the near clip plane, recomputes the z shift / scale and marks the camera frame dirty; returns the camera. */
void *RwCameraSetNearClipPlane(void *cam, float v)
{
  FLD(cam, float, 0x80) = v;
  CameraUpdateZShiftScale(cam);
  if (FLD(cam, void *, 4)) RwFrameUpdateObjects(FLD(cam, void *, 4));
  return cam;
}
