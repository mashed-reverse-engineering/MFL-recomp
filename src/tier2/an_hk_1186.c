#include "mfl_tier2.h"
/*005baf80*/ extern void FUN_005baf80(undefined4 p0, undefined4 p1, undefined4 p2);
/*005beec0*/ extern int __cdecl FUN_005beec0(void *a1);
/*007dae38*/ extern undefined4 g_data_7dae38;
/*007dae3c*/ extern undefined g_data_7dae3c;
/*007dae60*/ extern undefined4 g_data_7dae60;
/*007dae64*/ extern undefined4 g_data_7dae64;
/* gap_5bb060 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* no context */

void gap_5bb060(void)

{
  g_data_7dae38 = 1;
  FUN_005baf80(g_data_7dae60,0,0);
  g_data_7dae60 = 0;
  FUN_005beec0(&g_data_7dae3c);
  g_data_7dae64 = 0;
  return;
}


