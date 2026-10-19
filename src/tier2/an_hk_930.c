#include "mfl_tier2.h"
/* FUN_004acef0 */

/* "COM adjustor thunk: this - 8 is the outer object, forwards to its vtable slot +0x8" */

int __stdcall
FUN_004acef0(int *self)

{
  int *obj;
  
  obj = (int *)self[-2];
  return (*(int (__stdcall **)(int *))(*obj + 0x8))(obj);
}
