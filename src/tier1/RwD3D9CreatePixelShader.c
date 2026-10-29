/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/* Creates a pixel shader on the device; TRUE on success. */
int RwD3D9CreatePixelShader(void *src, void *out)
{
  return ((int (__stdcall *)(ComObj *, void *, void *))g_d3d9_device->vtbl[0x1a8 / 4])(g_d3d9_device, src, out) >= 0;
}
