#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl block_pool_create(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007dbb68*/ extern undefined g_data_7dbb68;
/*007dbb8c*/ extern undefined4 g_data_7dbb8c;
/* gap_5be660 */

/* no context */

undefined4 gap_5be660(void)

{
  int iVar1;
  
  iVar1 = block_pool_create(0x10,8,4,0,&g_data_7dbb68,0x40802);
  if (iVar1 != 0) {
    g_data_7dbb8c = 1;
    return 1;
  }
  return 0;
}
