/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpPipelineState { char pad[0xc]; int count; } RwpPipelineState;
typedef struct RwpPipeline { char pad[0x70]; RwpPipelineState *state; } RwpPipeline;
extern void *rwp_batch_0057c1e0(void *batch);
/* Runs the batch item pass (rwp_batch_0057c1e0) when the pipeline state has entries; returns the pipeline. (Rwp library) */
RwpPipeline *rwp_pipeline_00571970(RwpPipeline *p)
{
  if (p->state->count != 0) rwp_batch_0057c1e0(p);
  return p;
}
