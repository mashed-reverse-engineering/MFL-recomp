/* compile: /G5 */
#include "mfl_tier2.h"
/*007da868*/ extern int g_rp_7da868;
/*00405e40*/ extern void *__cdecl stub_return_zero(void *a);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Geometry plugin: stores the result of the (stub) patch query in the plugin slot; returns TRUE. */
int rpPatchGeometryAlways(void *o)
{
  char *e = (char *)o + g_rp_7da868;
  FLD(e, void *, 4) = stub_return_zero(o);
  return 1;
}
