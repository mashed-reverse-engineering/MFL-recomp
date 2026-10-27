#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c4100*/ extern undefined g_particles_6c4100;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5040*/ extern undefined g_particles_6c5040;
/*006c5080*/ extern undefined g_particles_6c5080;
/*006c50c0*/ extern undefined4 g_particles_6c50c0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* particles_render_all_systems */

/* "sets g_particles_6c50c0 from the camera, renders the five systems without z write" */

void particles_render_all_systems(void)
{
  if (RwCameraGetCurrentCamera() != (void *)0x0) {
    g_particles_6c50c0 = *(int *)((char *)RwCameraGetCurrentCamera() + 4) + 0x40;
  }
  else {
    g_particles_6c50c0 = 0;
  }
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  particles_render(&g_particles_6c40c0);
  particles_render(&g_particles_6c5000);
  particles_render(&g_particles_6c4100);
  particles_render(&g_particles_6c5040);
  particles_render(&g_particles_6c5080);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}
