/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/*0062e768*/ extern int g_rp_62e768;
/* Sets the vertex shader unless it is already current; caches it on success. */
void rwD3D9SetVertexShader(int s)
{
  if (g_rp_62e768 != s) { int hr = ((int (__stdcall *)(ComObj *, int))g_d3d9_device->vtbl[0x170 / 4])(g_d3d9_device, s); g_rp_62e768 = -1; if (hr >= 0) g_rp_62e768 = s; }
}
