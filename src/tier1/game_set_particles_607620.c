#include "mfl_tier2.h"
/*00607620*/ extern undefined4 g_particles_607620;


/* game_set_particles_607620 */

/* Sets the global particle system pointer or state. */
void game_set_particles_607620(undefined4 particles)
{
  g_particles_607620 = particles;
  return;
}
