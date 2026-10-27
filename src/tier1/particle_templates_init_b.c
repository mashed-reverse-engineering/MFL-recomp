#include "mfl_tier2.h"
/*00485ae0*/ extern int particle_templates_init(void);
/* Duplicate entry point that tail-calls particle_templates_init. */
int particle_templates_init_b(void)
{
  return particle_templates_init();
}
