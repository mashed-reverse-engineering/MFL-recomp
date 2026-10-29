/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/* Sets vertex shader constants on the D3D9 device (virtual call +0x178). */
int rwD3D9SetVertexShaderConstant(int reg, void *data, int n)
{
  return ((int (__stdcall *)(ComObj *, int, void *, int))g_d3d9_device->vtbl[0x178 / 4])(g_d3d9_device, reg, data, n);
}
