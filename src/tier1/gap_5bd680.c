#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl block_pool_create(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007db7c8*/ extern undefined g_data_7db7c8;
/*007db7ec*/ extern char *g_data_7db7ec;
/*007db7f0*/ extern char *g_data_7db7f0;
/*007db7f4*/ extern undefined4 g_data_7db7f4;
/* gap_5bd680 */

/* no context */

undefined4 gap_5bd680(void)

{
  int iVar1;
  
  g_data_7db7ec = (char *)(&g_data_7db7ec);
  g_data_7db7f0 = (char *)(&g_data_7db7ec);
  iVar1 = block_pool_create(0x28,0x10,4,0,&g_data_7db7c8,0x4080a);
  if (iVar1 != 0) {
    g_data_7db7f4 = 1;
    return 1;
  }
  return 0;
}
