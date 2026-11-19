#include "mfl_tier2.h"
/*0075cdbc*/ extern undefined g_particles_75cdbc;
/*0075cdcc*/ extern undefined g_particles_75cdcc;


/* particle_slot_call_render_method */

/* "calls virtual method 0x48 of the slot object when both tables are set" */

void particle_slot_call_render_method(int param_1)

{
  if ((*(int *)(&g_particles_75cdcc + param_1 * 4) != 0) &&
     (*(int *)(&g_particles_75cdbc + param_1 * 4) != 0)) {
                    /* WARNING: Could not recover jumptable at 0x0049c90e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(char **)(&g_particles_75cdcc + param_1 * 4) + 0x48))
              (*(int *)(&g_particles_75cdcc + param_1 * 4));
    return;
  }
  return;
}
