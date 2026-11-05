/* rwa_stream_find_chunk_005bb750 (0x005bb750): RWA binary streams (_rwaRwStreamFindChunkSync): skip chunks until one of the given type; it must be of a supported version (0x35000 .. 0x37002) */
#include "mfl_tier2.h"

/*005bb6c0*/ extern int _rwaRwStreamReadChunkHeaderInfoSync(void *stream, void *header, int swap);
/*004dc190*/ extern void *RwStreamSkip(void *stream, RwUInt32 offset);

typedef struct RwaChunkHeaderInfo { RwUInt32 type, length, version, buildNum, isComplex; } RwaChunkHeaderInfo;

#define RWA_VERSION_MIN 0x35000
#define RWA_VERSION_MAX 0x37002

/* size / version: optional outputs; swap: unused.  Returns 1 when found. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_find_chunk_005bb750(void *stream, RwUInt32 type, RwUInt32 *size, RwUInt32 *version, int swap)
{
  RwaChunkHeaderInfo info;

  if (!_rwaRwStreamReadChunkHeaderInfoSync(stream, &info, 0))
    return 0;
  while (info.type != type) {
    if (!RwStreamSkip(stream, info.length) || !_rwaRwStreamReadChunkHeaderInfoSync(stream, &info, 0))
      return 0;
  }
  if (info.version < RWA_VERSION_MIN || info.version > RWA_VERSION_MAX)
    return 0;
  if (size)
    *size = info.length;
  if (version)
    *version = info.version;
  return 1;
}
