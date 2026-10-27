#include "mfl_tier2.h"
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0062ac74*/ extern float g_car_62ac74;
/* gap_4575c0 */

/* no context */

void gap_4575c0(char *param_1)

{
  fx_emitter_spawn_a_default
            (*(int *)(*(char **)(param_1 + 0xc138) + 4) + 0x40,&g_car_62ac74,0x43b40000);
  return;
}


