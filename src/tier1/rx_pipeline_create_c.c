#include "mfl_tier2.h"
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004def80*/ extern struct RxPipeline * RxPipelineCreate();
/*004e64f0*/ extern struct RxPipeline * RxLockedPipeUnlock(RxLockedPipe *p0);
/*004e6f40*/ extern RxLockedPipe * RxPipelineLock(struct RxPipeline *p0);
/*004e7100*/ extern RxLockedPipe * RxLockedPipeAddFragment(RxLockedPipe *p0, RwUInt32 *p1, struct RxNodeDefinition *p2);


/* rx_pipeline_create_c */

/* "RxPipelineCreate + lock + nodes (variant c)" */

RxPipeline * __cdecl rx_pipeline_create_c(RxNodeDefinition *nodeDef0)
{
  RxPipeline *pipeline;
  RxLockedPipe *pRVar1;
  RxPipeline *pRVar2;
  
  pipeline = RxPipelineCreate();
  if (pipeline != (RxPipeline *)0x0) {
    pRVar1 = RxPipelineLock(pipeline);
    if (pRVar1 != (RxLockedPipe *)0x0) {
      pRVar1 = RxLockedPipeAddFragment(pRVar1,(RwUInt32 *)0x0,nodeDef0,0);
      pRVar2 = RxLockedPipeUnlock(pRVar1);
      if (pRVar2 != (RxPipeline *)0x0) {
        return pipeline;
      }
    }
    _rxPipelineDestroy_004deff0(pipeline);
  }
  return (RxPipeline *)0x0;
}
