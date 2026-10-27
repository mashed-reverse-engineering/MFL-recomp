#include "mfl_tier2.h"
/*004a68d0*/ extern int d3d9_pipeline_create_a();
/*004a68f0*/ extern int d3d9_pipeline_create_b();
/*006c3eb0*/ extern undefined4 g_object_set_6c3eb0;
/*006c3eb4*/ extern undefined4 g_object_set_6c3eb4;
/*006c3ebc*/ extern undefined4 g_data_6c3ebc;
/* material_plugin_register_cb_481b30 */

/* function pointer pushed/stored by material_plugin_register (PUSH 0x481b30) */

undefined4 material_plugin_register_cb_481b30(undefined4 param_1)

{
  if (g_data_6c3ebc != 0) {
    g_data_6c3ebc = g_data_6c3ebc + 1;
    return param_1;
  }
  g_data_6c3ebc = g_data_6c3ebc + 1;
  g_object_set_6c3eb4 = d3d9_pipeline_create_b();
  if ((g_object_set_6c3eb4 != 0) &&
     (g_object_set_6c3eb0 = d3d9_pipeline_create_a(), g_object_set_6c3eb0 != 0)) {
    return param_1;
  }
  return 0;
}


