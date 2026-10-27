#include "mfl_tier2.h"
/*005c4340*/ extern int __cdecl FUN_005c4340(void *a1, int a2, int a3);
/*007daf48*/ extern undefined4 g_data_7daf48;
/*007daf4c*/ extern undefined4 g_data_7daf4c;
/*007daf50*/ extern char *g_data_7daf50;
/*007daf54*/ extern char *g_data_7daf54;
/*007daf58*/ extern undefined4 g_data_7daf58;
/*007db760*/ extern undefined4 g_data_7db760;
/*007db764*/ extern undefined4 g_data_7db764;
/*007db768*/ extern undefined4 g_data_7db768;
/*007db76c*/ extern undefined4 g_data_7db76c;
/*007db770*/ extern undefined4 g_data_7db770;
/* gap_5bb7e0 */

/* no context */

undefined4 gap_5bb7e0(void)

{
  g_data_7db760 = 0;
  g_data_7db764 = 0;
  g_data_7db768 = 0xffffffff;
  g_data_7db76c = 0;
  FUN_005c4340(&g_data_7db770,1,1);
  g_data_7daf58 = 0;
  g_data_7daf50 = (char *)(&g_data_7daf50);
  g_data_7daf54 = (char *)(&g_data_7daf50);
  FUN_005c4340(&g_data_7daf4c,1,1);
  FUN_005c4340(&g_data_7daf48,1,1);
  return 1;
}


