#include "mfl_tier2.h"
/* fx_object_timer_subtract */
/* "(object, dt): object +0x5C -= dt" */
void __cdecl fx_object_timer_subtract(char *object,float dt,char *unused_3,char *unused_4,char *unused_5)
{
  *(float *)(object + 0x5c) = *(float *)(object + 0x5c) - dt;
  return;
}
