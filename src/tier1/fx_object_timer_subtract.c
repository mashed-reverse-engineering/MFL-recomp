#include "mfl_tier2.h"
/* fx_object_timer_subtract */
/* "(object, dt): object +0x5C -= dt" */
void __cdecl fx_object_timer_subtract(char *param_1,float param_2,char *param_3,char *param_4,char *param_5)
{
  *(float *)(param_1 + 0x5c) = *(float *)(param_1 + 0x5c) - param_2;
  return;
}
