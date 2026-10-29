/* compile: /G5 */
#include "mfl_tier2.h"
/*005850c0*/ extern void __cdecl FUN_005850c0(void);
/*00585160*/ extern int __cdecl FUN_00585160(void);
/* Runs FUN_005850c0, then returns 2 when FUN_00585160 is non-zero, 1 otherwise. (Rwp library, proposed name rwp_batch_005782f0) */
int FUN_005782f0(void)
{
  FUN_005850c0();
  return (FUN_00585160() != 0) + 1;
}
