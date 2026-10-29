#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl block_pool_create(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007db778*/ extern undefined g_data_7db778;
/*007db79c*/ extern undefined4 g_data_7db79c;
/* gap_5bbfe0 */

/* no context */

undefined4 gap_5bbfe0(void)

{
  int iVar1;
  
  iVar1 = block_pool_create(0x5c,0x10,4,0,&g_data_7db778,0x40806);
  if (iVar1 != 0) {
    g_data_7db79c = 1;
    return 1;
  }
  return 0;
}
