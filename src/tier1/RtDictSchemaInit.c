/* compile: /G5 */
#include "mfl_tier2.h"
extern void *__cdecl rwSListCreate(int size, int flags);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Creates the schema's simple list (4 byte entries); returns the schema or NULL. */
void *RtDictSchemaInit(void *s)
{
  void *l = rwSListCreate(4, FLD(s, unsigned, 4) | 0x40000);
  FLD(s, void *, 0x10) = l;
  if (!l) return 0;
  FLD(s, int, 0x14) = 0;
  return s;
}
