/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct ComObj { void **vtbl; } ComObj;
/*007d2240*/ extern ComObj *g_d3d9_device;
extern void (__cdecl *g_rx_7d53c4)(void);
/*0062e76c*/ extern int g_rp_62e76c;
/* Sets the pixel shader unless it is already current; caches it on success. */
void rwD3D9SetPixelShader(int s)
{
  if (g_rp_62e76c != s) { int hr = ((int (__stdcall *)(ComObj *, int))g_d3d9_device->vtbl[0x1ac / 4])(g_d3d9_device, s); g_rp_62e76c = -1; if (hr >= 0) g_rp_62e76c = s; }
}
