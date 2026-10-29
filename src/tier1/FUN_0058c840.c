/* compile: /G5 */
#include "mfl_tier2.h"
/*005f7350*/ extern char g_data_5f7350[];
/*0056c950*/ extern void __cdecl FUN_0056c950(void *a, void *b);
/* Rwp null shape constructor: base constructor with the null vtable. (Rwp library, proposed name rwp_null_0058c840) */
void *FUN_0058c840(void *o)
{
  FUN_0056c950(o, g_data_5f7350);
  return o;
}
