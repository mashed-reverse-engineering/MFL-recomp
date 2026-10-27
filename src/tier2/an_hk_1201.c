#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl FUN_005bef40(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007dbb90*/ extern char *g_data_7dbb90;
/*007dbb94*/ extern char *g_data_7dbb94;
/*007dbb98*/ extern undefined g_data_7dbb98;
/*007dbbbc*/ extern undefined4 g_data_7dbbbc;
/* gap_5be8a0 */

/* no context */

undefined4 gap_5be8a0(void)

{
  int iVar1;
  
  g_data_7dbb90 = (char *)(&g_data_7dbb90);
  g_data_7dbb94 = (char *)(&g_data_7dbb90);
  iVar1 = FUN_005bef40(0x1c,8,4,0,&g_data_7dbb98,0x40800);
  if (iVar1 != 0) {
    g_data_7dbbbc = 1;
    return 1;
  }
  return 0;
}


