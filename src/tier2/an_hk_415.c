#include "mfl_tier2.h"
extern char LAB_004a6720[];
/*004a6270*/ extern int d3d9_shaders_init_once();
/*004a6b40*/ extern void * rx_pipeline_create_for_node_5009c0();
/*004a6b50*/ extern undefined __cdecl rx_pipeline_set_callback_5009c0(struct RxPipeline *p0, RxD3D9AllInOneRenderCallBack p1);
/* d3d9_pipeline_create_b */

/* "d3d9_shaders_init_once + pipeline with render callback 004A6720" */

/* Creates the D3D9 render pipeline with the all-in-one render callback installed.
   Takes no parameters. Returns the new pipeline. */
RxPipeline * d3d9_pipeline_create_b(void)

{
  RxPipeline *pipeline;

  /* Initialize the D3D9 shaders (only does work on the first call, inferred from the name) */
  d3d9_shaders_init_once();
  pipeline = (RxPipeline *)rx_pipeline_create_for_node_5009c0();
  /* Install the render callback at LAB_004a6720 (the all-in-one D3D9 callback) */
  rx_pipeline_set_callback_5009c0(pipeline,(RxD3D9AllInOneRenderCallBack)&LAB_004a6720);
  return pipeline;
}


