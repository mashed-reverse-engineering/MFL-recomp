/* compile: /G5 */
#include "mfl_tier2.h"
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the camera view offset (two floats) and marks its frame dirty; returns the camera. */
void *RwCameraSetViewOffset(void *cam, int *off)
{
  FLD(cam, int, 0x78) = off[0];
  FLD(cam, int, 0x7c) = off[1];
  if (FLD(cam, void *, 4)) RwFrameUpdateObjects(FLD(cam, void *, 4));
  return cam;
}
