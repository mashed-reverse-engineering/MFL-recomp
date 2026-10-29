/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/* Creates a 16 bit managed index buffer of n indices on the D3D9 device; TRUE on success. */
int RwD3D9IndexBufferCreate(int n, void *out)
{
  return ((int (__stdcall *)(ComObj *, int, int, int, int, void *, int))g_d3d9_device->vtbl[0x6c / 4])(g_d3d9_device, n * 2, 8, 0x65, 1, out, 0) >= 0;
}
