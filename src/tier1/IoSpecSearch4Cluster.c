/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct IoSpec { unsigned id, a, b; } IoSpec;
typedef struct IoSpecList { unsigned n; IoSpec *e; } IoSpecList;
/* Index of the entry with the given id in a 12-byte-entry table, -1 when absent. */
unsigned IoSpecSearch4Cluster(IoSpecList *l, unsigned id)
{
  unsigned i;
  for (i = 0; i < l->n; i++) { if (l->e[i].id == id) return i; }
  return -1;
}
