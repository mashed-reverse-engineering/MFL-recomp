/* compile: /G5 */
#include "mfl_tier2.h"
extern void *FUN_0056a1d0(void *pool);
extern void rwp_obj_0056e8f0(void *o, void *a, int b, int c, int d);
/* Takes an object from the pool and initialises it. (Rwp library) */
void *rwp_obj_0056e890(void *pool, int b, int c, int d)
{
  void *o = FUN_0056a1d0(pool);
  rwp_obj_0056e8f0(o, pool, b, c, d);
  return o;
}
