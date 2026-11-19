#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*006f6910*/ extern undefined4 g_data_6f6910;
/*006f6918*/ extern undefined4 g_data_6f6918;
/*006f6920*/ extern undefined4 g_data_6f6920;
/*006f6928*/ extern undefined4 g_data_6f6928;
/*006f6930*/ extern undefined4 g_rw_6f6930;
/*006f6934*/ extern undefined4 g_data_6f6934;
/*006f693c*/ extern undefined4 g_ptank_atomic;
/*006f6944*/ extern undefined4 g_ptank_lock_6f6944;
/*007da6bc*/ extern char *g_ptank_plugin_offset;
/*0088a844*/ extern undefined4 g_rw_88a844;


/* ptank_lock_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RpPTankAtomicLock for the three property sets" */

void ptank_lock_all(undefined4 param_1)

{
  int iVar2;
  
  g_ptank_lock_6f6944 = 1;
  RpPTankAtomicLock_00545d10(g_ptank_atomic,&g_data_6f6918,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_ptank_atomic,&g_data_6f6910,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_ptank_atomic,&g_data_6f6928,4,0x40000000);
  RpPTankAtomicLock_00545d10(g_ptank_atomic,&g_data_6f6920,0x80,0x40000000);
  iVar2 = 0;
  g_data_6f6934 = 0;
  if (0 < (int)g_rw_6f6930) {
    do {
      func_0x0048dc40(param_1,g_rw_88a844 + iVar2 * 8);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)g_rw_6f6930);
  }
  RpPTankAtomicUnlock_00545f20(g_ptank_atomic);
  if (0x800 < (int)g_data_6f6934) {
    g_data_6f6934 = 0x7ff;
  }
  *(uint *)(*(char **)(g_ptank_plugin_offset + g_ptank_atomic) + 0x40) |= 0x800000;
  *(int *)(*(char **)(g_ptank_plugin_offset + g_ptank_atomic) + 4) = g_data_6f6934;
  return;
}
