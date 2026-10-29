/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/*007d26a4*/ extern void *g_rw_7d26a4;
/* Sets the D3D9 depth stencil surface unless it is already current. */
void rwD3D9SetDepthStencilSurface(void *s)
{
  if (g_rw_7d26a4 != s) { g_rw_7d26a4 = s; ((int (__stdcall *)(ComObj *, void *))g_d3d9_device->vtbl[0x9c / 4])(g_d3d9_device, s); }
}
