/* compile: /G5 */
#include "mfl_tier2.h"
/*004d5bf0*/ extern void __cdecl RwTextureDestroy(void *tex);
/* Destroys a texture (RwTextureDestroy); returns it. */
void *destroyTexture(void *t)
{
  RwTextureDestroy(t);
  return t;
}
