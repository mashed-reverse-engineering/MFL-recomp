#include "mfl_tier2.h"
extern char LAB_004a9ff0[];
/*004a6ad0*/ extern void * rx_pipeline_create_for_node_5003b0();
/*004a6b10*/ extern undefined __cdecl rx_pipeline_set_callback_5003b0(struct RxPipeline *p0, RxD3D9AllInOneRenderCallBack p1);
/*00771030*/ extern undefined4 g_app_771030;
/* d3d9_pipeline_create_default */
/* "creates the default RxPipeline and sets its render callback"
 * Stores the new pipeline in g_app_771030 and points its render callback at LAB_004a9ff0.
 * Returns 1 (inferred: always succeeds; the result of the create call is not checked). */
undefined4 d3d9_pipeline_create_default(void)
{
  g_app_771030 = (RxPipeline *)rx_pipeline_create_for_node_5003b0();
  rx_pipeline_set_callback_5003b0(g_app_771030,(RxD3D9AllInOneRenderCallBack)&LAB_004a9ff0);
  return 1;
}
