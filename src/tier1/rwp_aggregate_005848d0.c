/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpAggregate { char pad[0x40]; void *primitive; } RwpAggregate;
extern int rwp_aggregate_005895c0(void *primitive, int a, int b);
/* Forwards a query on an aggregate to its primitive (rwp_aggregate_005895c0). (Rwp library) */
int rwp_aggregate_005848d0(RwpAggregate *aggregate, int a, int b)
{
  return rwp_aggregate_005895c0(aggregate->primitive, a, b);
}
