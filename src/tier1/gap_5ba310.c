#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl guid_compare(int a1, int a2);
/* gap_5ba310 */

/* no context */

bool gap_5ba310(char *unused_1,undefined4 *param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = guid_compare(*param_2,param_3);
  return iVar1 == 0;
}
