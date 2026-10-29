/* compile: /G5 */
#include "mfl_tier2.h"
/*005d6030*/ extern int __cdecl rwSListDestroy(void *l);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Destroys the simple list held by a dictionary schema; returns TRUE. */
int RtDictSchemaDestruct(void *s)
{
  rwSListDestroy(FLD(s, void *, 0x10));
  return 1;
}
