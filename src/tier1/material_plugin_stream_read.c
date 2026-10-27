#include "mfl_tier2.h"
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*0089de04*/ extern undefined4 g_app_89de04;
#define MATERIAL_PLUGIN_STREAM_SIZE (0x30)
#define MATERIAL_PLUGIN_DWORDS (0xc)
/* material_plugin_stream_read */

/* Plugin stream reader for the material: reads MATERIAL_PLUGIN_STREAM_SIZE (0x30) bytes from stream into a local
   buffer and copies them as MATERIAL_PLUGIN_DWORDS dwords to g_app_89de04 + plugin_offset.
   Params: stream = input stream, param_2 = unused, plugin_offset = material plugin block offset from g_app_89de04 (inferred).
   Returns stream on success, NULL when the read is short. */

/* TIER2: validated 2026-10-18 authority=diffemu */
typedef struct { undefined4 d[12]; } Dw12m;
RwStream * material_plugin_stream_read(RwStream *stream,undefined4 param_2,char *plugin_offset)
{
  RwUInt32 bytes_read;
  undefined4 *dst;
  undefined4 buffer [MATERIAL_PLUGIN_DWORDS];

  dst = (undefined4 *)(g_app_89de04 + plugin_offset);
  bytes_read = RwStreamRead(stream,buffer,MATERIAL_PLUGIN_STREAM_SIZE);
  if (bytes_read != MATERIAL_PLUGIN_STREAM_SIZE) {
    return (RwStream *)0x0;
  }
  *(Dw12m *)dst = *(Dw12m *)buffer;
  return stream;
}
