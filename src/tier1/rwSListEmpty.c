/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Empties a simple list (entry count = 0). */
void rwSListEmpty(void *o)
{
  FLD(o, int, 4) = 0;
}
