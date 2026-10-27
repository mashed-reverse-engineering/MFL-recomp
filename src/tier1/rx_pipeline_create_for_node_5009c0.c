#include "mfl_tier2.h"
/*004a6a80*/ extern int __cdecl rx_pipeline_create_c(struct RxNodeDefinition *p0);
/*005009c0*/ extern char *FUN_005009c0(void);
/* rx_pipeline_create_for_node_5009c0 */
/* "rx_pipeline_create_c with the node definition FUN_005009c0" */
/* Takes no parameters and returns nothing. Gets the node definition from FUN_005009c0 (inferred: builds it)
   and passes it to rx_pipeline_create_c, which creates the pipeline for that node. */
int rx_pipeline_create_for_node_5009c0(void)
{
  RxNodeDefinition *node_def;

  node_def = (RxNodeDefinition *)FUN_005009c0();
  return rx_pipeline_create_c(node_def);
}
