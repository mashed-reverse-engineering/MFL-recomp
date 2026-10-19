#include "mfl_tier2.h"
extern char LAB_004a6a40[];
extern char LAB_004a6a60[];
/*004a69a0*/ extern void * rx_pipeline_create_a();
/*004a69f0*/ extern void * rx_pipeline_create_b();
/*004e7070*/ extern struct RxPipelineNode * RxPipelineFindNodeByName(struct RxPipeline *p0, RwChar *p1, struct RxPipelineNode *p2, RwInt32 *p3);
/*004fb460*/ extern void RxD3D9AllInOneSetRenderCallBack(struct RxPipelineNode *p0, RxD3D9AllInOneRenderCallBack p1);
/*005003b0*/ extern int FUN_005003b0(void);
/*005009c0*/ extern int FUN_005009c0(void);
/*00771044*/ extern undefined4 g_app_boot_771044;
/*00771048*/ extern undefined4 g_app_771048;
/*0077104c*/ extern undefined4 g_app_boot_77104c;
/*00771054*/ extern undefined4 g_app_771054;
/* render_pipelines_create_two */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "creates two RxPipelines and installs the render callbacks 0x4a6a40/0x4a6a60" */
/* Creates two RxPipelines, stored in g_app_771054 and g_app_771048. In each one it finds the
   node named by a string from FUN_005003b0 / FUN_005009c0 (inferred), saves the node's old
   render callback in g_app_boot_771044 / g_app_boot_77104c (inferred from the name), and
   installs LAB_004a6a40 / LAB_004a6a60 as the new callback. No parameters, no return value. */

void render_pipelines_create_two(void)

{
  undefined4 *name_ref;
  RxPipelineNode *node;
  
  /* pipeline A: create it, find the node named by FUN_005003b0 (inferred), save that node's
     current render callback in g_app_boot_771044, install LAB_004a6a40 as the new callback */
  g_app_771054 = (RxPipeline *)rx_pipeline_create_a();
  name_ref = (undefined4 *)FUN_005003b0();
  node = RxPipelineFindNodeByName
                     (g_app_771054,(RwChar *)*name_ref,(RxPipelineNode *)0x0,(RwInt32 *)0x0);
  g_app_boot_771044 = RxD3D9AllInOneGetRenderCallBack(node);
  RxD3D9AllInOneSetRenderCallBack(node,(RxD3D9AllInOneRenderCallBack)&LAB_004a6a40);
  /* pipeline B: same steps as pipeline A, with FUN_005009c0 (inferred) and LAB_004a6a60 */
  g_app_771048 = (RxPipeline *)rx_pipeline_create_b();
  name_ref = (undefined4 *)FUN_005009c0();
  node = RxPipelineFindNodeByName
                     (g_app_771048,(RwChar *)*name_ref,(RxPipelineNode *)0x0,(RwInt32 *)0x0);
  g_app_boot_77104c = RxD3D9AllInOneGetRenderCallBack(node);
  RxD3D9AllInOneSetRenderCallBack(node,(RxD3D9AllInOneRenderCallBack)&LAB_004a6a60);
  return;
}


