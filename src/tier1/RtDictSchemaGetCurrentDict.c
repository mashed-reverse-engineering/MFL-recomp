/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Current dictionary of a dictionary schema. */
void *RtDictSchemaGetCurrentDict(void *s)
{
  return FLD(s, void *, 0x14);
}
