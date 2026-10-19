#include "mfl_tier2.h"
/*0040ab40*/ extern int led_file_name();
/*004dbfc0*/ extern struct RwStream * RwStreamWrite(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc6c0*/ extern struct RwStream * _rwStreamWriteVersionedChunkHeader(struct RwStream *p0, RwInt32 p1, RwInt32 p2, RwUInt32 p3, RwUInt32 p4);
/*00651ac0*/ extern undefined4 g_line_exceptions;
/*00652cc0*/ extern undefined1 g_led_file_name;

/* LE<n>.LED file index passed to led_file_name() (inferred from the LE<n>.LED comment below) */
#define LE_FILE_INDEX (2)
/* chunk type id of the line exceptions table chunk */
#define LINE_EXCEPTIONS_CHUNK_ID (0x13269901)
/* table size in bytes: 0x480 dwords */
#define LINE_EXCEPTIONS_SIZE (0x1200)
/* RenderWare library version and build number written in the chunk header (inferred) */
#define RW_LIBRARY_VERSION (0x37002)
#define RW_BUILD_NUMBER (0x24)

/* line_exceptions_save */

/* "writes the 0x480 dword table to LE<n>.LED with chunk 0x13269901 (counterpart of
   line_exceptions_load)" */

/* Returns 1 if the table was written to the LE file, 0 if the file could not be opened or the write failed (inferred from the code). */

/* TIER2: unvalidated 2026-10-18 authority=diffemu */
undefined4 line_exceptions_save(void)
{
  RwStream *le_stream;
  RwStream *written_stream;

  led_file_name(LE_FILE_INDEX);
  le_stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMWRITE,&g_led_file_name);
  if (le_stream != (RwStream *)0x0) {
    /* chunk header, then the table itself */
    _rwStreamWriteVersionedChunkHeader(le_stream,LINE_EXCEPTIONS_CHUNK_ID,LINE_EXCEPTIONS_SIZE,
                                       RW_LIBRARY_VERSION,RW_BUILD_NUMBER);
    written_stream = RwStreamWrite(le_stream,&g_line_exceptions,LINE_EXCEPTIONS_SIZE);
    if (written_stream != (RwStream *)0x0) {
      RwStreamClose(le_stream,(void *)0x0);
      return 1;
    }
  }
  return 0;
}


