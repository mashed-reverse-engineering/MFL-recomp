/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/*0062e768*/ extern int g_rp_62e768;
/* Creates a vertex shader on the device; resets the last-used shader cache on success; TRUE on success. */
int RwD3D9CreateVertexShader(void *src, void *out)
{
  int hr = ((int (__stdcall *)(ComObj *, void *, void *))g_d3d9_device->vtbl[0x16c / 4])(g_d3d9_device, src, out);
  if (hr >= 0) g_rp_62e768 = -1;
  return hr >= 0;
}
