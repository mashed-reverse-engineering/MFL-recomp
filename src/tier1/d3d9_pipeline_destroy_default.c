#include "mfl_tier2.h"
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*00771030*/ extern undefined4 g_app_771030;


/* d3d9_pipeline_destroy_default */

/* "_rxPipelineDestroy(g_app_771030)" */

undefined4 d3d9_pipeline_destroy_default(void)
{
  _rxPipelineDestroy_004deff0(g_app_771030);
  return 1;
}
