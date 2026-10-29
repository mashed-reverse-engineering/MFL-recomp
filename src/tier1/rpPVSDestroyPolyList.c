/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Frees a linked list of PVS polygon lists (next pointer at +0x58). */
void rpPVSDestroyPolyList(void **l)
{
  void *p;
  while ((p = *l) != 0) { *l = FLD(p, void *, 0x58); RwFree(p); }
}
