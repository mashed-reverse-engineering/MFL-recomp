/* compile: /G5 */
#include "mfl_tier2.h"
/*007da7b0*/ extern int g_rp_7da7b0;
extern void __cdecl DefaultGeomAnimCB(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Geometry plugin constructor: clears the geometry animation data and installs the default animation callback; returns the object. */
void *GeomAnimCtor(void *o)
{
  char *e = (char *)o + g_rp_7da7b0;
  FLD(e, int, 0) = 0;
  FLD(e, int, 8) = 0;
  FLD(e, void *, 4) = DefaultGeomAnimCB;
  return o;
}
