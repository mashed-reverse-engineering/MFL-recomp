/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *FnI3)(void *, void *, int);
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Clears the camera through the device callback; returns the camera or NULL. */
void *RwCameraClear(void *cam, void *rgba, int mode)
{
  FnI3 f = ENG(0x9c, FnI3);
  return f(cam, rgba, mode) ? cam : 0;
}
