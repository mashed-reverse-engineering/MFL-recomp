#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*006a1820*/ extern undefined g_course_car_6a1820;
/*006a1860*/ extern undefined4 g_course_car_6a1860;
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*006a1820*/ extern undefined g_course_car_6a1820;
/*006a1860*/ extern undefined4 g_course_car_6a1860;
/* course_car_shadow_particles_create */

/* "car_shadow texture, particles_create(g_particles_6a1860, 0x80c, 0x50)" */

void course_car_shadow_particles_create(void)
{
  RwTexture *texture_dict;
  
  g_course_car_6a1860 = (uint *)&g_course_car_6a1820;
  texture_dict = (RwTexture *)find_texture_dict_b("car_shadow");
  particle_system_setup_ptank(g_course_car_6a1860,0x80c,0x50,texture_dict);
  return;
}
