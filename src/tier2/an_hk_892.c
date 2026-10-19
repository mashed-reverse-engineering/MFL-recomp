#include "mfl_tier2.h"
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
/* Size in bytes of one atomic plugin data block as stored in the stream. */
#define ATOMIC_PLUGIN_DATA_SIZE (0x2c)
/* atomic_plugin_stream_read */

/* "reads 0x2C bytes of the atomic plugin data" */

/* Reads one atomic plugin data block from `stream` and copies its bytes 0x08..0x1f
   (six words) to the same offsets of the plugin slot at g_gfx_89de0c + dest_offset (inferred).
   `unused_arg` is not referenced by the body (inferred).
   Returns `stream` on success, NULL if the stream did not yield a full block. */
RwStream * atomic_plugin_stream_read(RwStream *stream,char *unused_arg,int dest_offset)

{
  int dest;
  RwUInt32 bytes_read;
  struct {
    undefined1 raw [8];
    undefined4 word_08;
    undefined4 word_0c;
    undefined4 word_10;
    undefined4 word_14;
    undefined4 word_18;
    undefined4 word_1c;
    int pad_top[3];
  } frame;

  dest = g_gfx_89de0c + dest_offset;
  /* Read the whole block; a short read means the stream is exhausted or corrupt. */
  bytes_read = RwStreamRead(stream,frame.raw,ATOMIC_PLUGIN_DATA_SIZE);
  if (bytes_read != ATOMIC_PLUGIN_DATA_SIZE) {
    return (RwStream *)0x0;
  }
  /* Copy the six words after the 8-byte header into the destination slot. */
  *(RwV3d *)(dest + 8) = *(RwV3d *)&frame.word_08;
  *(RwV3d *)(dest + 0x14) = *(RwV3d *)&frame.word_14;
  return stream;
}


