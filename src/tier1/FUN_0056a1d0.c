/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpStack2 { char pad[0x38]; void **items; int count; } RwpStack2;
typedef struct RwpPooled { void *owner; unsigned short slot; } RwpPooled;
/* Pops an object from the second Rwp pool stack (items at +0x38, count at +0x3c), sets its owner and marks its slot 0xffff; NULL when empty. (Rwp library, proposed name rwp_dyn_0056a1d0) */
RwpPooled *FUN_0056a1d0(RwpStack2 *s)
{
  int n = s->count;
  RwpPooled *o;
  if (n == 0) return 0;
  n--;
  s->count = n;
  o = (RwpPooled *)s->items[n];
  o->owner = s;
  o->slot = 0xffff;
  return o;
}
