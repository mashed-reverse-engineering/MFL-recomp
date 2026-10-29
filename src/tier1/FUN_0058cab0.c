/* compile: /G5 */
#include "mfl_tier2.h"
/*0056c350*/ extern int __cdecl FUN_0056c350(void *a, void *b, int c, void *d);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Forwards the object's first two members and a second argument to FUN_0056c350 with the constant 1. */
int FUN_0058cab0(void *a, void *b)
{
  char *x = FLD(a, char *, 0x10);
  return FUN_0056c350(FLD(x, void *, 0), FLD(x, void *, 8), 1, b);
}
