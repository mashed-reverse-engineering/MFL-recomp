#include "mfl_tier2.h"
/*005bef40*/ extern int __cdecl block_pool_create(int a1, int a2, int a3, int a4, void *a5, int a6);
/*007dbbc0*/ extern char *g_data_7dbbc0;
/*007dbbc4*/ extern char *g_data_7dbbc4;
/*007dbbc8*/ extern undefined g_data_7dbbc8;
/*007dbbec*/ extern undefined4 g_data_7dbbec;
/*007dbbf0*/ extern undefined4 g_data_7dbbf0;
/* gap_5bf070 */

/* no context */

undefined4 gap_5bf070(void)

{
  int iVar1;
  
  g_data_7dbbc0 = (char *)(&g_data_7dbbc0);
  g_data_7dbbc4 = (char *)(&g_data_7dbbc0);
  g_data_7dbbec = 1;
  iVar1 = block_pool_create(0x24,0x10,4,0,&g_data_7dbbc8,0x40800);
  g_data_7dbbf0 = iVar1;
  if (iVar1 == 0) {
    g_data_7dbbec = iVar1;
    return 0;
  }
  **(undefined4 **)(iVar1 + 0x20) = *(undefined4 *)(iVar1 + 0x1c);
  *(undefined4 *)(*(char **)(iVar1 + 0x1c) + 4) = *(undefined4 *)(iVar1 + 0x20);
  return 1;
}


