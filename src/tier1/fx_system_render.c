#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();

#define FX_SYSTEM_PARTICLES_OFFSET 0x10
#define FX_SYSTEM_ACTIVE_FLAG_OFFSET 0x54

/* Renders the particles of the FX system if active, then clears the active flag. */
void fx_system_render(char *fx_system)
{
  /* Check if system is active */
  if (*(int *)(fx_system + FX_SYSTEM_ACTIVE_FLAG_OFFSET) != 0) {
    particles_render(fx_system + FX_SYSTEM_PARTICLES_OFFSET);
    *(undefined4 *)(fx_system + FX_SYSTEM_ACTIVE_FLAG_OFFSET) = 0;
  }
  return;
}

