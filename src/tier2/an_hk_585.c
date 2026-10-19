#include "mfl_tier2.h"
/*004960e0*/ extern int particle_system_set_current();
/* byte offset in game_state of the current particle system pointer (inferred) */
#define PARTICLE_SYSTEM_OFFSET (51668)
/* particle_system_set_current_forward */

/* "calls particle_system_set_current" */

/* Makes the particle system stored in game_state (at PARTICLE_SYSTEM_OFFSET) the current one,
   passing set_arg through unchanged. param_3 is unused. Returns nothing.
   game_state and set_arg are inferred names. */
void __cdecl particle_system_set_current_forward(char *game_state,undefined4 set_arg,char *param_3)

{
  particle_system_set_current(*(int *)((char *)game_state + PARTICLE_SYSTEM_OFFSET), set_arg);
  return;
}


