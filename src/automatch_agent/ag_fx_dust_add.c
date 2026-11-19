#include "mfl_tier2.h"
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486b50*/ extern int particles_add();
/*00759648*/ extern int g_fx_dust_759648;
/*00759658*/ extern undefined g_fx_dust_759658;
/*00759660*/ extern int g_fx_dust_759660;


/* fx_dust_add */

/* "adds one dust quad when the pool has room (alpha in the high byte)" */

void fx_dust_add(undefined4 matrix,byte alpha)

{
  RwRGBA color;
  
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  color.alpha = alpha;
  if (g_fx_dust_759648 < g_fx_dust_759660) {
    gfx_set_fx_matrix(matrix);
    particles_set_color(&color);
    particles_add(&g_fx_dust_759658);
    g_fx_dust_759648 = g_fx_dust_759648 + 1;
  }
  return;
}
