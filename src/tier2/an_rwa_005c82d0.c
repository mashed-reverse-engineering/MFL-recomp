/* rwa_stream_node_set_output_connection_005c82d0 (0x005c82d0): stream node: connect its output, the connection's callback is the node's write function (5c8100) */
#include "mfl_tier2.h"

/*005c1e30*/ extern void _rwaStreamNodeObjSetOutputConnection(char *node, int index, void **connection);
/*005c8100*/ extern void gap_5c8100(void);      /* the node's write function */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_stream_node_set_output_connection_005c82d0(char *node, RwUInt32 param, void **connection)
{
  _rwaStreamNodeObjSetOutputConnection(node, 0, connection);
  *(void **)((char *)connection[1] + 8) = connection[0];
  *(void **)((char *)connection[1] + 4) = (void *)gap_5c8100;
  return node;
}
