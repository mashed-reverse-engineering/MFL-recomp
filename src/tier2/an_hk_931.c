#include "mfl_tier2.h"
/* FUN_004aced0 */

/* "COM adjustor thunk: this - 8 is the outer object, forwards to its vtable slot +0x0" */

int __stdcall
FUN_004aced0(int *self, void *riid, void *ppv)

{
  int *obj;
  
  obj = (int *)self[-2];
  return (*(int (__stdcall **)(int *, void *, void *))(*obj + 0x0))(obj, riid, ppv);
}
