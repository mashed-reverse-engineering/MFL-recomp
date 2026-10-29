#include "mfl_tier2.h"
/*004a6290*/ extern int d3d9_pixel_shader_destroy();
/*004a6ae0*/ extern void _rxPipelineDestroy_004deff0(struct RxPipeline *p0);
/*006c3eb0*/ extern char *g_object_set_6c3eb0;
/*006c3eb4*/ extern char *g_object_set_6c3eb4;
/*006c3eb8*/ extern char *g_object_set_6c3eb8;
/*006c3ebc*/ extern undefined4 g_data_6c3ebc;
/* material_plugin_register_cb_481ac0 */

/* function pointer pushed/stored by material_plugin_register (PUSH 0x481ac0) */

undefined4 material_plugin_register_cb_481ac0(undefined4 param_1)

{
  g_data_6c3ebc = g_data_6c3ebc + -1;
  if (g_data_6c3ebc == 0) {
    if (g_object_set_6c3eb0 != 0) {
      _rxPipelineDestroy_004deff0((struct RxPipeline *)g_object_set_6c3eb0);
      g_object_set_6c3eb0 = (char *)(RxPipeline *)0x0;
    }
    if (g_object_set_6c3eb4 != 0) {
      _rxPipelineDestroy_004deff0((struct RxPipeline *)g_object_set_6c3eb4);
      g_object_set_6c3eb4 = (char *)(RxPipeline *)0x0;
    }
    if (g_object_set_6c3eb8 != 0) {
      _rxPipelineDestroy_004deff0((struct RxPipeline *)g_object_set_6c3eb8);
      g_object_set_6c3eb8 = (char *)(RxPipeline *)0x0;
    }
    d3d9_pixel_shader_destroy();
  }
  return param_1;
}
