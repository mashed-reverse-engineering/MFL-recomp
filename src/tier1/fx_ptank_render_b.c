#include "mfl_tier2.h"
/*0075969c*/ extern char *g_fx_ptank_atomic_b;


/* fx_ptank_render_b */

/* "renders g_particles_75969c" */

void fx_ptank_render_b(void)
{
  (**(code **)(g_fx_ptank_atomic_b + 0x48))(g_fx_ptank_atomic_b);
  return;
}
