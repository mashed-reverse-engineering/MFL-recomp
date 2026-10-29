/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/*0058cbe0*/ extern void __cdecl FUN_0058cbe0(void *o);
/*0056e550*/ extern void __cdecl FUN_0056e550(void *o);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys an Rwp body: FUN_0056e550, FUN_0058cbe0 on its member, then frees it; returns TRUE. */
int FUN_0058ca00(void *o)
{
  FUN_0056e550(o);
  FUN_0058cbe0(FLD(o, void *, 4));
  RwFree(o);
  return 1;
}
