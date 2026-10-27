#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00653258*/ extern undefined g_trail_653258;


/* trail_particles_create */

/* "particles_create(0x653258, 0x37, 0x14, texture) with blend modes 5 / 6" */

void trail_particles_create(void)
{
  RwTexture *texture_dict;
  
  texture_dict = (RwTexture *)find_texture_dict_b("VehicleIcons");
  particle_system_setup_ptank((uint *)&g_trail_653258,0x37,0x14,texture_dict);
  ptank_set_blend_modes(&g_trail_653258,5,6);
  return;
}
