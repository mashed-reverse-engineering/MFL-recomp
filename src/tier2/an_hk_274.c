#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0062942c*/ extern undefined4 g_particle_templates_62942c;
/*006c4140*/ extern undefined g_gfx_6c4140;
/*006c5000*/ extern char g_particles_6c5000[];

#define PARTICLE_TEMPLATE_SIZE (0xec)
#define PARTICLE_TEMPLATE_DEFAULT_OFFSET (0xcc)

/* particle_templates_init */

/* Initialises the particle template table at g_gfx_6c4140, up to g_particles_6c5000.
 * Each PARTICLE_TEMPLATE_SIZE-byte template is zeroed, then the 32-bit word at
 * PARTICLE_TEMPLATE_DEFAULT_OFFSET inside it is set to g_particle_templates_62942c
 * (default value, inferred). Takes no parameters, returns nothing. */

void particle_templates_init(void)

{
  undefined *template_ptr;

  /* walk the table one template at a time until the end of the region */
  template_ptr = &g_gfx_6c4140;
  do {
    mem_zero(template_ptr,PARTICLE_TEMPLATE_SIZE);
    *(undefined4 *)(template_ptr + PARTICLE_TEMPLATE_DEFAULT_OFFSET) = g_particle_templates_62942c;
    template_ptr = template_ptr + PARTICLE_TEMPLATE_SIZE;
  } while ((LONG_PTR)template_ptr < ((LONG_PTR)&g_particles_6c5000));
  return;
}


