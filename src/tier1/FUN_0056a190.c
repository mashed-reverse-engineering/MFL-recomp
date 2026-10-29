/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpStack { char pad[0x28]; void **items; int count; } RwpStack;
/* Pops the top pointer of an Rwp pointer stack (items at +0x28, count at +0x2c); NULL when empty. (Rwp library, proposed name rwp_dyn_0056a190) */
void *FUN_0056a190(RwpStack *s)
{
  int n = s->count;
  if (n == 0) return 0;
  n--;
  s->count = n;
  return s->items[n];
}
