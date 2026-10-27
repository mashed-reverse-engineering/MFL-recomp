#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl FUN_005bef40(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007dab28*/ extern char *g_data_7dab28;
/*007dab2c*/ extern char *g_data_7dab2c;
/*007dab30*/ extern undefined g_data_7dab30;
/*007dab54*/ extern undefined4 g_data_7dab54;
/* gap_5b71a0 */

/* no context */

void gap_5b71a0(void)

{
  int iVar1;
  
  g_data_7dab28 = (char *)(&g_data_7dab28);
  g_data_7dab2c = (char *)(&g_data_7dab28);
  iVar1 = FUN_005bef40(0x18,0x40,4,0,&g_data_7dab30,0x4080b);
  if (iVar1 == 0) {
    return;
  }
  g_data_7dab54 = 1;
  return;
}


