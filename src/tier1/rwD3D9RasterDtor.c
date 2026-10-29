/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*00947144*/ extern int g_rw_947144;
/* Raster plugin destructor: releases the D3D9 surface and clears the pointer; returns the raster. */
void *rwD3D9RasterDtor(void *r)
{
  ComObj **slot = (ComObj **)((char *)r + g_rw_947144 + 0x1c);
  if (*slot) { ComObj *o = *slot; ((void (__stdcall *)(ComObj *))o->vtbl[2])(o); *slot = 0; }
  return r;
}
