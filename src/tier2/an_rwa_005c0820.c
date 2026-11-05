/* rwa_stream_container_get_chain_param_005c0820 (0x005c0820): stream containers: parameter 1 of a chain object of a running container (state 5): the last one for index > 1, else the index-th from the first */
#include "mfl_tier2.h"

/*005c1ee0*/ extern int rwa_obj_int_get_param_data_005c1ee0(void *obj, int intf, int index);

typedef struct RwaChainLink {      /* at +0x2c of a chain object */
  struct RwaChainLink *prev;       /* +0x00 */
  struct RwaChainLink *next;       /* +0x04 */
  char pad[0x24];
  int intf;                        /* +0x2c: the object's interface (obj+0x58) */
} RwaChainLink;

#define CONTAINER_RUNNING 5
#define CHAIN_OBJ(l) ((char *)(l) - 0x2c)

/* query+8: the chain index.  Returns the parameter, 0 when the container is not running. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_container_get_chain_param_005c0820(char *container, char *query)
{
  RwUInt32 index = *(RwUInt32 *)(query + 8);
  RwaChainLink *l;

  if (*(int *)(container + 8) != CONTAINER_RUNNING)
    return 0;
  if (index & ~1u)
    l = *(RwaChainLink **)(container + 0x20);      /* the last chain */
  else {
    l = *(RwaChainLink **)(container + 0x24);      /* the first chain */
    while (--index)      /* (index 0 walks 2^32 - 1 links, as the original) */
      l = l->next;
  }
  return rwa_obj_int_get_param_data_005c1ee0(CHAIN_OBJ(l), l->intf, 1);
}
