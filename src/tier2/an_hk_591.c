#include "mfl_tier2.h"
/*0049cd60*/ extern undefined fx_spawn_burst(float *p1, int p2, float *p3, undefined4 *p4, undefined4 *p5, float p6);
/* fx_spawn_burst_default_scale */

/* "fx_spawn_burst(..., 1.0f)" */

void fx_spawn_burst_default_scale
               (float *param_1,int param_2,char *param_3,undefined4 *param_4,
               undefined4 *param_5,float param_6)

{
  fx_spawn_burst(param_1,param_2,(float *)param_3,param_4,param_5,param_6);
  return;
}


