#include "mfl_tier2.h"
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004def80*/ extern struct RxPipeline * RxPipelineCreate();
/*004e64f0*/ extern struct RxPipeline * RxLockedPipeUnlock(RxLockedPipe *p0);
/*004e6f40*/ extern RxLockedPipe * RxPipelineLock(struct RxPipeline *p0);
/*004e7100*/ extern RxLockedPipe * RxLockedPipeAddFragment(RxLockedPipe *p0, RwUInt32 *p1, struct RxNodeDefinition *p2, ...);
/*005003b0*/ extern char *FUN_005003b0(void);
/* rx_pipeline_create_a */
/* "RxPipelineCreate + lock + nodes (variant a)" */
/*
 * Creates a render pipeline and adds one node fragment to it while locked.
 * Takes no parameters. Returns the pipeline on success, or NULL on failure
 * (a partly built pipeline is destroyed first).
 */
RxPipeline * rx_pipeline_create_a(void)
{
  RxPipeline *pipeline;
  RxLockedPipe *locked;
  RxNodeDefinition *node_def;
  RxPipeline *unlocked;

  pipeline = RxPipelineCreate();
  if (pipeline != (RxPipeline *)0x0) {
    /* Lock the pipeline so fragments can be added */
    locked = RxPipelineLock(pipeline);
    if (locked != (RxLockedPipe *)0x0) {
      /* Get the node definition (inferred: FUN_005003b0 returns it) and add it as a fragment */
      node_def = (RxNodeDefinition *)FUN_005003b0();
      locked = RxLockedPipeAddFragment(locked,(RwUInt32 *)0x0,node_def,0);
      /* Unlock; a NULL result means the unlock failed */
      unlocked = RxLockedPipeUnlock(locked);
      if (unlocked != (RxPipeline *)0x0) {
        return pipeline;
      }
    }
    _rxPipelineDestroy_004deff0(pipeline);
  }
  return (RxPipeline *)0x0;
}
