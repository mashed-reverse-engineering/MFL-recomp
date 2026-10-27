#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl FUN_005bef40(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007db7a0*/ extern undefined g_data_7db7a0;
/*007db7c4*/ extern undefined4 g_data_7db7c4;
/* gap_5bcbb0 */

/* no context */

undefined4 gap_5bcbb0(void)

{
  int iVar1;
  
  g_data_7db7c4 = 1;
  iVar1 = FUN_005bef40(0x5c,8,4,0,&g_data_7db7a0,0x40805);
  if (iVar1 != 0) {
    return 1;
  }
  g_data_7db7c4 = 0;
  return 0;
}


