#include "mfl_tier2.h"
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*00771048*/ extern undefined4 g_app_771048;
/*00771054*/ extern undefined4 g_app_771054;


/* d3d9_pipelines_destroy */

/* "destroys g_app_771054 and g_app_771048" */

void d3d9_pipelines_destroy(void)
{
  _rxPipelineDestroy_004deff0(g_app_771054);
  _rxPipelineDestroy_004deff0(g_app_771048);
  return;
}
