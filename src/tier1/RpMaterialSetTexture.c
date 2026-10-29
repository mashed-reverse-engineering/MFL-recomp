/* compile: /G5 */
#include "mfl_tier2.h"
/*004d5bf0*/ extern void __cdecl RwTextureDestroy(void *t);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets a material's texture: takes a reference on the new one and destroys the old one; returns the material. */
void *RpMaterialSetTexture(void *m, void *t)
{
  if (t) FLD(t, int, 0x54)++;
  if (FLD(m, void *, 0)) RwTextureDestroy(FLD(m, void *, 0));
  FLD(m, void *, 0) = t;
  return m;
}
