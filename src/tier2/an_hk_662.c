#include "mfl_tier2.h"
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*00564e40*/ extern int Rt2dFontStreamRead();
#define RT2D_FONT_CHUNK_ID (0x199)
/* rt2d_stream_find_chunk */

/* "opens a stream and finds chunk 0x199 (Rt2d font / brush data)" */

/* Opens the RenderWare file stream named by filename, finds the Rt2d font chunk
   (RT2D_FONT_CHUNK_ID) in it, reads the font data with Rt2dFontStreamRead and closes
   the stream. Returns the value of Rt2dFontStreamRead (inferred: a font handle),
   or 0 if the stream could not be opened or the chunk is missing. */
undefined4 rt2d_stream_find_chunk(void *filename)

{
  RwStream *stream;
  RwBool found;
  undefined4 result;
  RwStream *stream_copy;

  result = 0;
  /* Open the file for reading; NULL means it could not be opened. */
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,filename);
  if (stream != (RwStream *)0x0) {
    /* Search for the font chunk; no chunk size/offset output is requested (NULL). */
    found = RwStreamFindChunk(stream,RT2D_FONT_CHUNK_ID,(RwUInt32 *)0x0,(RwUInt32 *)0x0);
    if (found != 0) {
      /* Unused: a second copy of stream (no effect on the result). */
      stream_copy = stream;
      /* Read the font data from the stream positioned at the chunk, then close it. */
      result = Rt2dFontStreamRead(stream);
      RwStreamClose(stream,(void *)0x0);
    }
  }
  return result;
}


