#include "mfl_tier2.h"
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);
/*005beec0*/ extern int __cdecl FUN_005beec0(void *a1);
/*007dad30*/ extern undefined g_data_7dad30;
/*007dad58*/ extern undefined g_data_7dad58;
/*007dadb4*/ extern undefined g_data_7dadb4;
/*007dadd8*/ extern undefined g_data_7dadd8;
/*007dadfc*/ extern undefined4 g_data_7dadfc;
/*007dae00*/ extern undefined4 g_data_7dae00;
/* gap_5ba7e0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* no context */

void gap_5ba7e0(void)

{
  g_data_7dadfc = 1;
  FUN_005ba420(&g_data_7dad58);
  FUN_005beec0(&g_data_7dadb4);
  FUN_005beec0(&g_data_7dadd8);
  FUN_005beec0(&g_data_7dad30);
  g_data_7dae00 = 0;
  return;
}


