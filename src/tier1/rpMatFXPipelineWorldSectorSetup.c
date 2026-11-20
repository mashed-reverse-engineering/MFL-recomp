/* compile: /G5 */
#include "mfl_tier2.h"
/*007da738*/ extern int g_rp_7da738;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the MatFX sector pipeline in the sector's pipeline field.
   Returns the sector; 0x550161 tests it (VOID_w16: was void). */
void *rpMatFXPipelineWorldSectorSetup(void *a)
{
  FLD(a, int, 0x7c) = g_rp_7da738;
  return a;
}
