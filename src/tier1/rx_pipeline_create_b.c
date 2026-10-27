#include "mfl_tier2.h"
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*004def80*/ extern struct RxPipeline * RxPipelineCreate();
/*004e64f0*/ extern struct RxPipeline * RxLockedPipeUnlock(RxLockedPipe *p0);
/*004e6f40*/ extern RxLockedPipe * RxPipelineLock(struct RxPipeline *p0);
/*004e7100*/ extern RxLockedPipe * RxLockedPipeAddFragment(RxLockedPipe *p0, RwUInt32 *p1, struct RxNodeDefinition *p2, ...);
/*005009c0*/ extern char *FUN_005009c0(void);
/* rx_pipeline_create_b */
/* "RxPipelineCreate + lock + nodes (variant b)" */
/*
 * Creates a render pipeline and adds one node fragment to it while it is locked.
 * The node definition comes from FUN_005009c0 (inferred: it returns the node
 * definition to add). Takes no parameters.
 * Returns the new pipeline on success, or NULL after destroying the partially
 * built pipeline if creating, locking, adding the fragment or unlocking fails.
 */
RxPipeline * rx_pipeline_create_b(void)
{
  RxPipeline *pipeline;
  RxLockedPipe *locked_pipe;
  RxNodeDefinition *node_def;
  RxPipeline *unlocked_pipe;
  
  pipeline = RxPipelineCreate();
  if (pipeline != (RxPipeline *)0x0) {
    /* Lock the pipeline and add the node fragment; the fragment call returns the still-locked pipe */
    locked_pipe = RxPipelineLock(pipeline);
    if (locked_pipe != (RxLockedPipe *)0x0) {
      node_def = (RxNodeDefinition *)FUN_005009c0();
      locked_pipe = RxLockedPipeAddFragment(locked_pipe,(RwUInt32 *)0x0,node_def,0);
      unlocked_pipe = RxLockedPipeUnlock(locked_pipe);
      if (unlocked_pipe != (RxPipeline *)0x0) {
        return pipeline;
      }
    }
    /* Any step failed: destroy the pipeline, which is not returned to the caller */
    _rxPipelineDestroy_004deff0(pipeline);
  }
  return (RxPipeline *)0x0;
}
