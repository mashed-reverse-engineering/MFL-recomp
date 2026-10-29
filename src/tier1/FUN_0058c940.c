/* compile: /G5 */
#include "mfl_tier2.h"
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *f);
/*0058cad0*/ extern void __cdecl FUN_0058cad0(void *o);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls FUN_0058cad0 on the object and marks its frame dirty when it has one; returns the object. */
void *FUN_0058c940(void *o)
{
  FUN_0058cad0(o);
  if (FLD(o, void *, 0x30)) RwFrameUpdateObjects(FLD(FLD(o, void *, 0x30), void *, 4));
  return o;
}
