/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/*007d3050*/ extern int g_rw_7d3050[];
/* Sets a sampler state when it differs from the cached value (14 states per stage). */
void RwD3D9SetSamplerState(int stage, int type, int value)
{
  int idx = type + stage * 14;
  if (g_rw_7d3050[idx] != value) { g_rw_7d3050[idx] = value; ((void (__stdcall *)(ComObj *, int, int, int))g_d3d9_device->vtbl[0x114 / 4])(g_d3d9_device, stage, type, value); }
}
