/* rwa_stream_source_set_output_connection_005c8d20 (0x005c8d20): _rwaStreamSourceSetOutputConnection */
#include "mfl_tier2.h"

/*005c1e30*/ extern void _rwaStreamNodeObjSetOutputConnection(void *node, int index, void *connection);      /* attach the connection to the node */
/*005c8a40*/ extern int _rwaStreamSourceGetStreamPacket();      /* the source's packet request callback */

/* After the generic attach the connection's callback record (+4) gets the source's request callback (+4) and the
   source itself as its context (+8).  The second argument (output index) is not used: the source has one output. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_stream_source_set_output_connection_005c8d20(void *source, int output, char *connection)
{
  _rwaStreamNodeObjSetOutputConnection(source, 0, connection);
  *(void **)(*(char **)(connection + 4) + 4) = (void *)_rwaStreamSourceGetStreamPacket;
  *(void **)(*(char **)(connection + 4) + 8) = source;
  return source;
}
