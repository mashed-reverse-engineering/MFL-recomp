#include "mfl_tier2.h"
extern char LAB_004a6860[];
/*004a6270*/ extern int d3d9_shaders_init_once();
/*004a6ad0*/ extern void * rx_pipeline_create_for_node_5003b0();
/*004a6b10*/ extern undefined __cdecl rx_pipeline_set_callback_5003b0(struct RxPipeline *p0, RxD3D9AllInOneRenderCallBack p1);
/* d3d9_pipeline_create_a */
/* "d3d9_shaders_init_once + pipeline with render callback 004A6860" */
/* Creates the D3D9 render pipeline: initialises the D3D9 shaders (once),
 * creates a pipeline for the node, and installs the render callback at
 * LAB_004a6860 (inferred: the all-in-one D3D9 render callback).
 * Takes no parameters. Returns the new pipeline. */
RxPipeline * d3d9_pipeline_create_a(void)
{
  RxPipeline *pipeline;
  
  d3d9_shaders_init_once();
  pipeline = (RxPipeline *)rx_pipeline_create_for_node_5003b0();
  rx_pipeline_set_callback_5003b0(pipeline,(RxD3D9AllInOneRenderCallBack)&LAB_004a6860);
  return pipeline;
}
