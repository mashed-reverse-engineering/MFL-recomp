/* compile: /G5 */
#include "mfl_tier2.h"
extern void FUN_00575b80(void *p);
/* Releases a buffer (rwp_capsule_00575b80) and returns 1. (Rwp library) */
int rwp_dyn_0056a170(void *p)
{
  FUN_00575b80(p);
  return 1;
}
