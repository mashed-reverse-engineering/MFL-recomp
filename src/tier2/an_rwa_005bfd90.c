/* rwa_stream_node_packet_done_005bfd90 (0x005bfd90): a stream node finished one packet of a connection */
#include "mfl_tier2.h"

extern __declspec(dllimport) long __stdcall InterlockedIncrement(long *addend);
extern __declspec(dllimport) long __stdcall InterlockedDecrement(long *addend);

/*005c1f30*/ extern int FUN_005c1f30(void *node, void *connection, int index);      /* node status query */

/* counters = { done, pending }: when the node (connection at +0x28) answers 1 the packet counts as done; it is no
   longer pending in any case. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_node_packet_done_005bfd90(char *node, long *counters)
{
  if (FUN_005c1f30(node, *(void **)(node + 0x28), 0) == 1)
    InterlockedIncrement(counters);
  InterlockedDecrement(counters + 1);
}
