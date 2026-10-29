/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the lighting callback (slot 3) in the pipeline's data block. */
void rxD3D9SkinVertexShaderSetLightingCallBack(void *p, void *cb)
{
  FLD(p, void **, 0x14)[3] = cb;
}
