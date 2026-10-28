#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl block_pool_create(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007dbb64*/ extern undefined4 g_data_7dbb64;
/*00949d20*/ extern undefined g_data_949d20;
/* gap_5be450 */

/* no context */

undefined4 gap_5be450(void)

{
  int iVar1;
  
  iVar1 = block_pool_create(0xc,0x20,0,0,&g_data_949d20,0x40804);
  if (iVar1 != 0) {
    g_data_7dbb64 = 1;
    return 1;
  }
  return 0;
}


