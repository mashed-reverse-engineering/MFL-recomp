/* compile: /G5 */
#include "mfl_tier2.h"
extern int g_rw_7d54a0;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Initialises the D3D9 light plugin data (index -1, 1.0, 0, 5.0); returns the light or NULL. */
void *rpD3D9LightConstructor(void *light)
{
  char *e;
  if (light) { e = (char *)light + g_rw_7d54a0; FLD(e, int, 0) = -1; FLD(e, float, 4) = 1.0f; FLD(e, int, 8) = 0; FLD(e, float, 0xc) = 5.0f; return light; }
  return 0;
}
