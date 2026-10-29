/* compile: /G5 */
#include "mfl_tier2.h"
/*0056a1b0*/ extern int __cdecl FUN_0056a1b0(void *a, void *b);
/* Pushes the object on the pointer stack stored in its first member; returns TRUE. (Rwp library, proposed name rwp_volume_0056b630) */
int FUN_0056b630(void **a)
{
  FUN_0056a1b0(*a, a);
  return 1;
}
