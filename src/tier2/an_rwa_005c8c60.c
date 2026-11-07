/* rwa_stream_source_set_pos_005c8c60 (0x005c8c60): stream sources: set the stream position of SOURCE (the second argument is unused); returns SOURCE */
#include "mfl_tier2.h"

/*005c8ae0*/ extern void rwa_stream_source_set_stream_pos_005c8ae0(char *source, RwUInt32 pos);

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
char *rwa_stream_source_set_pos_005c8c60(char *source, void *unused, RwUInt32 pos)
{
  rwa_stream_source_set_stream_pos_005c8ae0(source, pos);
  return source;
}
