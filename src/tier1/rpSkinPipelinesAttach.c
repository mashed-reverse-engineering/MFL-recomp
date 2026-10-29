/* compile: /G5 */
#include "mfl_tier2.h"
/*007da664*/ extern int g_rp_7da664;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stores the skin pipeline in the atomic's pipeline field. */
void rpSkinPipelinesAttach(void *a)
{
  FLD(a, int, 0x6c) = g_rp_7da664;
}
