#include "mfl_tier2.h"
/*005b7120*/ extern int __cdecl FUN_005b7120(void);
/*007dab58*/ extern char *g_data_7dab58;
/*007dab5c*/ extern char *g_data_7dab5c;
/*007dab60*/ extern char *g_data_7dab60;
/*007dab64*/ extern char *g_data_7dab64;
/*007dab6c*/ extern undefined4 g_data_7dab6c;
/* gap_5b7560 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* no context */

undefined4 gap_5b7560(void)

{
  g_data_7dab60 = (char *)(&g_data_7dab60);
  g_data_7dab64 = (char *)(&g_data_7dab60);
  g_data_7dab6c = 1;
  g_data_7dab58 = (char *)(&g_data_7dab58);
  g_data_7dab5c = (char *)(&g_data_7dab58);
  FUN_005b7120();
  return 1;
}


