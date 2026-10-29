/* compile: /G5 */
#include "mfl_tier2.h"
/*004deff0*/ extern void *__cdecl rxPipelineDestroy(void *p);
/*004dcdb0*/ extern void *__cdecl RwIm3DSetTransformPipeline(void *p);
/* Clears the Im3D transform pipeline and destroys the platform pipeline stored at the pointer. */
void rwIm3DDestroyPlatformTransformPipeline(void **p)
{
  RwIm3DSetTransformPipeline(0);
  if (*p) { rxPipelineDestroy(*p); *p = 0; }
}
