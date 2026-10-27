#include "mfl_tier2.h"
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);
/*005c44b0*/ extern int __cdecl FUN_005c44b0(void);
/*007dacb8*/ extern undefined g_data_7dacb8;
/*007dad2c*/ extern undefined4 g_data_7dad2c;
/* gap_5b9530 */

/* no context */

void gap_5b9530(void)

{
  int iVar1;
  
  if (g_data_7dad2c != 0) {
    iVar1 = FUN_005c44b0();
    if (iVar1 == 0) {
      return;
    }
    g_data_7dad2c = 0;
  }
  FUN_005ba420(&g_data_7dacb8);
  return;
}


