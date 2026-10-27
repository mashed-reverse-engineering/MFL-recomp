#include "mfl_tier2.h"
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);
/*005c8810*/ extern int __cdecl FUN_005c8810(void);
/*007dbef0*/ extern undefined g_data_7dbef0;
/* gap_5c8820 */

/* no context */

void gap_5c8820(void)

{
  int iVar1;
  
  iVar1 = FUN_005c8810();
  if (iVar1 == 0) {
    return;
  }
  FUN_005ba420(&g_data_7dbef0);
  return;
}


