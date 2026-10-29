/* compile: /G5 */
#include "mfl_tier2.h"
typedef void (__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Ends camera rendering through the camera's end-update callback (field +0x1c). */
void RwCameraEndUpdate(void *cam)
{
  FLD(cam, V1, 0x1c)(cam);
}
