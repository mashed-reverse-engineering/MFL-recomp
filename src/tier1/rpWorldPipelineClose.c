/* compile: /G5 */
#include "mfl_tier2.h"
extern void __cdecl rpDestroyPlatformWorldSectorPipelines(void);
extern void __cdecl rpDestroyPlatformAtomicPipelines(void);
/*0050b5e0*/ extern void __cdecl rxD3D9VertexShaderDefaultEndCallBack(void);
/* Destroys the platform sector and atomic pipelines and runs the (empty) end callback. */
void rpWorldPipelineClose(void)
{
  rpDestroyPlatformWorldSectorPipelines();
  rpDestroyPlatformAtomicPipelines();
  rxD3D9VertexShaderDefaultEndCallBack();
}
