#include "mfl_tier2.h"
/*004a6a80*/ extern undefined __cdecl rx_pipeline_create_c(struct RxNodeDefinition *p0);
/*005003b0*/ extern int FUN_005003b0(void);
/* rx_pipeline_create_for_node_5003b0 */

/* "rx_pipeline_create_c with the node definition FUN_005003b0" */

/* Gets the node definition from FUN_005003b0 (inferred) and creates the rx pipeline for it
   via rx_pipeline_create_c. Takes no parameters and returns nothing. */
void rx_pipeline_create_for_node_5003b0(void)

{
  RxNodeDefinition *node_def;

  node_def = (RxNodeDefinition *)FUN_005003b0();
  rx_pipeline_create_c(node_def);
  return;
}


