#include "mfl_tier2.h"
/*00771058*/ extern undefined4 g_app_771058;


/* d3d9_set_shader_flag */

/* "g_app_771058 = param" */

/* Store the D3D9 shader flag in the app global. */
void d3d9_set_shader_flag(undefined4 shader_flag)
{
  g_app_771058 = shader_flag;
  return;
}
