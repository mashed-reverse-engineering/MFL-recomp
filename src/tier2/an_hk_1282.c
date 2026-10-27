#include "mfl_tier2.h"
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);
/*005cdc10*/ extern int __cdecl FUN_005cdc10(void);
/*007dc068*/ extern undefined g_data_7dc068;
/*007dc0e4*/ extern undefined4 g_data_7dc0e4;
/* gap_5cb540 */

/* no context */

void gap_5cb540(void)

{
  int iVar1;
  
  if (g_data_7dc0e4 != 0) {
    iVar1 = FUN_005cdc10();
    if (iVar1 == 0) {
      return;
    }
    g_data_7dc0e4 = 0;
  }
  FUN_005ba420(&g_data_7dc068);
  return;
}


