/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
/* Frees an aligned block: the byte before the pointer holds the alignment padding. (Rwp library, proposed name rwp_capsule_00575b80) */
void FUN_00575b80(unsigned char *p)
{
  RwFree(p - p[-1]);
}
