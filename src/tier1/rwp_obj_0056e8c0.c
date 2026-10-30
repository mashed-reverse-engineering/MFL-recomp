/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpObj { void *pool; char pad[0x48]; void *link; } RwpObj;
extern void FUN_0056e750(void *link);
extern void FUN_0056a1f0(void *pool, RwpObj *o);
/* Releases the object's link and gives the object back to its pool; returns 1. (Rwp library) */
int rwp_obj_0056e8c0(RwpObj *o)
{
  FUN_0056e750(o->link);
  FUN_0056a1f0(o->pool, o);
  return 1;
}
