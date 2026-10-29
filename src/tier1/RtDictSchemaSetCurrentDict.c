/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the current dictionary of a schema; returns the schema. */
void *RtDictSchemaSetCurrentDict(void *s, void *d)
{
  FLD(s, void *, 0x14) = d;
  return s;
}
