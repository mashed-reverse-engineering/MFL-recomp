/* compile: /G5 */
#include "mfl_tier2.h"
/*0056a190*/ extern void *__cdecl FUN_0056a190(void *s);
/*0056b650*/ extern void __cdecl FUN_0056b650(void *o, void *s, int a, int b);
/* Pops an object from the stack and initialises it with two values (FUN_0056b650); returns the object or NULL. (Rwp library, proposed name rwp_volume_0056b600) */
void *FUN_0056b600(void *stack, int a, int b)
{
  void *o = FUN_0056a190(stack);
  if (o) FUN_0056b650(o, stack, a, b);
  return o;
}
