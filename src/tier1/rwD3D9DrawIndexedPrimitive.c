/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
extern void __cdecl rwD3D9RenderStateFlushCache(void);
/* Flushes the cached render states, then calls DrawIndexedPrimitive on the D3D9 device. */
int rwD3D9DrawIndexedPrimitive(int a, int b, int c, int d, int e, int f)
{
  rwD3D9RenderStateFlushCache();
  return ((int (__stdcall *)(ComObj *, int, int, int, int, int, int))g_d3d9_device->vtbl[0x148 / 4])(g_d3d9_device, a, b, c, d, e, f);
}
