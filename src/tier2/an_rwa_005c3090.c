/* rwa_stream_source_set_segment_005c3090 (0x005c3090): stream source (__rwaStreamSourceSetSegmentByName): current segment = entry (0x20 bytes) of the header's segment table */
#include "mfl_tier2.h"

/*005c2180*/ extern int __rwaStreamSourceGetSegmentIndexByUUID(char *source, void *uuid);

typedef struct RwaSegment { char bytes[0x20]; } RwaSegment;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_source_set_segment_005c3090(char *source, void *uuid)
{
  int i = __rwaStreamSourceGetSegmentIndexByUUID(source, uuid);
  RwaSegment *table = *(RwaSegment **)(*(char **)(source + 0x118) + 0x10);

  *(RwaSegment **)(source + 0x140) = table + i;
}
