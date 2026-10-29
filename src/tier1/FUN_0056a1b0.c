/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpStack { char pad[0x28]; void **items; int count; } RwpStack;
/* Pushes a pointer onto the Rwp pointer stack. (Rwp library, proposed name rwp_dyn_0056a1b0) */
void FUN_0056a1b0(RwpStack *s, void *v)
{
  s->items[s->count] = v;
  s->count = s->count + 1;
}
