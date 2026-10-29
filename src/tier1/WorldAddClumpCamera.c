/* compile: /G5 */
#include "mfl_tier2.h"
/*007d529c*/ extern int g_rp_7d529c;
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Marks the camera's frame dirty and stores the world in its world plugin slot; returns the camera. */
void *WorldAddClumpCamera(void *cam, void *world)
{
  char *e = (char *)cam + g_rp_7d529c;
  if (FLD(cam, void *, 4)) { RwFrameUpdateObjects(FLD(cam, void *, 4)); FLD(e, void *, 0xc) = world; return cam; }
  FLD(e, void *, 0xc) = world;
  return cam;
}
