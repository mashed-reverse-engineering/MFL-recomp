#include "mfl_tier2.h"
/*0040ab40*/ extern int led_file_name();
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*00651ac0*/ extern undefined4 g_line_exceptions;
/*00652cc0*/ extern undefined1 g_led_file_name;
#define LINE_EXCEPTIONS_CHUNK_ID (0x13269901)
/* line_exceptions_load */

/* Reads the line exception table from the LED file (g_led_file_name): calls led_file_name(1) first,
   opens the file, finds chunk LINE_EXCEPTIONS_CHUNK_ID and reads its payload into g_line_exceptions.
   Returns 1 on success, 0 if the file cannot be opened or the chunk is missing.
   (was: "led_load(1) then reads the LED chunk 0x13269901 into the line exception table (LE1.LED holds the
   AI line exceptions)") */

/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 line_exceptions_load(void)
{
  RwUInt32 chunk_size;
  RwUInt32 chunk_version;
  RwStream *stream;
  RwBool chunk_found;
  RwStream *stream_copy;
  undefined4 *exceptions_table;
  RwUInt32 chunk_size_copy;

  led_file_name(1);
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,&g_led_file_name);
  /* File opened: look for the line exception chunk (chunk_size / chunk_version are its length and version, inferred). */
  if (stream != (RwStream *)0x0) {
    chunk_found = RwStreamFindChunk(stream,LINE_EXCEPTIONS_CHUNK_ID,(RwUInt32 *)&chunk_size,
                              (RwUInt32 *)&chunk_version);
    /* Chunk found: read its payload straight into the table, then close the stream. */
    if (chunk_found != 0) {
      exceptions_table = &g_line_exceptions;
      stream_copy = stream;
      chunk_size_copy = chunk_size;
      RwStreamRead(stream,&g_line_exceptions,chunk_size);
      RwStreamClose(stream,(void *)0x0);
      return 1;
    }
  }
  return 0;
}


