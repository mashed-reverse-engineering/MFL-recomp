/* compile: /G5 */
#include "mfl_tier2.h"
typedef void *(__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Begins camera rendering through the camera's begin-update callback (field +0x18).
   Returns the callback's result (the camera, NULL on failure): 30 callers test it (VOID_w16: was void). */
void *RwCameraBeginUpdate(void *cam)
{
  return FLD(cam, V1, 0x18)(cam);
}
