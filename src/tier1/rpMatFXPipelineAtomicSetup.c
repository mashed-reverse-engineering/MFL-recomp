/* compile: /G5 */
#include "mfl_tier2.h"
/*007da734*/ extern int g_rp_7da734;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the MatFX atomic pipeline in the atomic's pipeline field.
   Returns the atomic; 0x550081 and 0x5501ae test it (VOID_w16: was void). */
void *rpMatFXPipelineAtomicSetup(void *a)
{
  FLD(a, int, 0x6c) = g_rp_7da734;
  return a;
}
