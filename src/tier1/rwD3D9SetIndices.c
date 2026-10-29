/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
/*0062e770*/ extern void **g_app_62e770;
/* Sets the D3D9 index buffer unless it is already current. */
void rwD3D9SetIndices(void *s)
{
  if (g_app_62e770 != s) { g_app_62e770 = s; ((int (__stdcall *)(ComObj *, void *))g_d3d9_device->vtbl[0x1a0 / 4])(g_d3d9_device, s); }
}
