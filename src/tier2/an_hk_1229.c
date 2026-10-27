#include "mfl_tier2.h"
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);
/*005ccfc0*/ extern int __cdecl FUN_005ccfc0(void);
/*007dc008*/ extern undefined g_data_7dc008;
/*007dc064*/ extern undefined4 g_data_7dc064;
/* gap_5c9c50 */

/* no context */

void gap_5c9c50(void)

{
  int iVar1;
  
  if (g_data_7dc064 != 0) {
    iVar1 = FUN_005ccfc0();
    if (iVar1 == 0) {
      return;
    }
    g_data_7dc064 = 0;
  }
  FUN_005ba420(&g_data_7dc008);
  return;
}


