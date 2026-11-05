/* rwa_stream_read_chunk_header_info_005bb6c0 (0x005bb6c0): RWA binary streams (_rwaRwStreamReadChunkHeaderInfoSync): read a RenderWare chunk header and unpack its library id into version / build */
#include "mfl_tier2.h"

/*004dbe70*/ extern RwUInt32 RwStreamRead(void *stream, void *buffer, RwUInt32 length);

typedef struct RwaChunkHeader { RwUInt32 type, length, libraryID; } RwaChunkHeader;
typedef struct RwaChunkHeaderInfo {
  RwUInt32 type;      /* +0x00 */
  RwUInt32 length;    /* +0x04 */
  RwUInt32 version;   /* +0x08 */
  RwUInt32 buildNum;  /* +0x0c */
  RwUInt32 isComplex; /* +0x10 */
} RwaChunkHeaderInfo;

/* The library id: old streams hold the plain version (< 0x10000), newer ones the packed form (version bits 14..31 / 16..21, build in the low word). */
#define LIBID_VERSION(id) (((((id) >> 14) & 0x3ff00) + 0x30000) | (((id) >> 16) & 0x3f))

/* swap: unused.  Returns the stream, NULL when the header could not be read. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_stream_read_chunk_header_info_005bb6c0(void *stream, RwaChunkHeaderInfo *info, int swap)
{
  RwaChunkHeader h;

  if (RwStreamRead(stream, &h, sizeof(h)) != sizeof(h))
    return 0;
  info->type = h.type;
  info->length = h.length;
  if (!(h.libraryID & 0xffff0000)) {
    info->version = h.libraryID << 8;
    info->buildNum = 0;
  } else {
    info->version = LIBID_VERSION(h.libraryID);
    info->buildNum = h.libraryID & 0xffff;
  }
  info->isComplex = 0;
  return stream;
}
