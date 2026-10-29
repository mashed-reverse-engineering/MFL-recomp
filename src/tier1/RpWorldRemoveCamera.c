/* compile: /G5 */
#include "mfl_tier2.h"
/*007d529c*/ extern int g_rp_7d529c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Clears the world link stored in the camera plugin data; returns the world, NULL when the camera was not in the world. */
void *RpWorldRemoveCamera(void *w, void *cam)
{
  char *p = (char *)cam + g_rp_7d529c;
  if (FLD(p, int, 0xc) != 0) { FLD(p, int, 0xc) = 0; FLD(p, int, 8) = 0; return w; }
  return 0;
}
