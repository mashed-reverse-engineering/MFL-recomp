/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpStack2 { char pad[0x38]; void **items; int count; } RwpStack2;
/* Pushes a pointer onto the second Rwp pool stack. (Rwp library, proposed name rwp_dyn_0056a1f0) */
void FUN_0056a1f0(RwpStack2 *s, void *v)
{
  s->items[s->count] = v;
  s->count = s->count + 1;
}
