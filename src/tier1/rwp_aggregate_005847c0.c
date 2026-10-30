/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpAggregate { char pad[0x40]; void *primitive; } RwpAggregate;
extern void rwp_aggregate_00589560(void *primitive);
/* Releases the primitive of an aggregate (rwp_aggregate_00589560); returns 1. (Rwp library) */
int rwp_aggregate_005847c0(RwpAggregate *a)
{
  rwp_aggregate_00589560(a->primitive);
  return 1;
}
