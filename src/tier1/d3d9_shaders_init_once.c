#include "mfl_tier2.h"
/*0077103c*/ extern undefined4 g_d3d9_shaders_77103c;
/* d3d9_shaders_init_once */
/*
 * Creates the D3D9 shaders the first time it is called; later calls do nothing.
 * Takes no parameters and returns nothing. The "done" flag is
 * g_d3d9_shaders_77103c (0 = not created yet, 1 = created).
 */
void d3d9_shaders_init_once(void)
{
  if (g_d3d9_shaders_77103c == 0) {
    d3d9_shaders_create();
    g_d3d9_shaders_77103c = 1;
  }
  return;
}
