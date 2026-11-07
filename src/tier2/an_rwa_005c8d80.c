/* rwa_stream_source_node_destroy_005c8d80 (0x005c8d80): stream sources: node destroy of a stream source: flush and destroy its source state (+0x5c), drop its interface (+0x58), then the generic stream node destroy */
#include "mfl_tier2.h"

/*005c2ea0*/ extern void rwa_stream_source_flush_005c2ea0(char *source);
/*005bd6c0*/ extern void FUN_005bd6c0(void *intf);      /* RwaObjInterfaceRemove */
/*005c2170*/ extern void rwa_stream_source_destroy_005c2170(char *source);
/*005c1d80*/ extern void *FUN_005c1d80(void *node, void *lookup, void *arg);      /* _RwaStreamNodeDestroy */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void *rwa_stream_source_node_destroy_005c8d80(char *node, void *lookup, void *arg)
{
  rwa_stream_source_flush_005c2ea0(node + 0x5c);
  FUN_005bd6c0(*(void **)(node + 0x58));
  rwa_stream_source_destroy_005c2170(node + 0x5c);
  return FUN_005c1d80(node, lookup, arg);
}
