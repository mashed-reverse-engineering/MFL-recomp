/* compile: /G5 */
#include "mfl_tier2.h"
/*007da71c*/ extern int g_rp_7da71c;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Effect type of a material's MatFX data, 0 when it has none. */
int RpMatFXMaterialGetEffects(void *m)
{
  void *p = FLD(m, void *, g_rp_7da71c);
  if (!p) return 0;
  return FLD(p, int, 0x30);
}
