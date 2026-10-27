#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*00453980*/ extern int course_particle_timers_reset();
/*00486930*/ extern int ptank_set_size();
/*00486a60*/ extern int ptank_set_colour();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0060d8a8*/ extern undefined4 g_course_smoke_60d8a8;
/*006a07d8*/ extern undefined g_course_smoke_6a07d8;
/* Smoke particle tank: size of the zeroed block, system id and particle count (inferred from the call below). */
#define PARTICLE_TANK_SIZE (0x1040)
#define SMOKE_PTANK_TYPE (0x895)
#define SMOKE_PTANK_MAX_PARTICLES (0x100)
/* course_smoke_particles_create */
/* "smoke texture, particles_create(0x6A07D8, 0x895, 0x100)" */
/*
 * Sets up the course smoke particle tank at g_course_smoke_6a07d8.
 * No parameters, no return value.
 * Zeroes the tank, creates it with the "smoke" texture, applies the size
 * stored in g_course_smoke_60d8a8 (inferred), sets colour 0, then resets
 * the course particle timers.
 */
void course_smoke_particles_create(void)
{
  RwTexture *texture_dict;
  struct {
    float width;
    float height;
  } particle_size;

  /* Size pair: both fields copied from the same global (inferred). */
  particle_size.width = *(float *)&g_course_smoke_60d8a8;
  particle_size.height = *(float *)&g_course_smoke_60d8a8;
  /* Clear the whole tank before setting it up. */
  mem_zero(&g_course_smoke_6a07d8,PARTICLE_TANK_SIZE);
  /* Look up the "smoke" texture dictionary entry and create the tank. */
  texture_dict = (RwTexture *)find_texture_dict_b("smoke");
  particle_system_setup_ptank((uint *)&g_course_smoke_6a07d8,SMOKE_PTANK_TYPE,SMOKE_PTANK_MAX_PARTICLES,texture_dict);
  /* Apply size and colour to the new tank, then reset the timers. */
  ptank_set_size(&g_course_smoke_6a07d8,&particle_size.width);
  ptank_set_colour(&g_course_smoke_6a07d8,0);
  course_particle_timers_reset();
  return;
}
