#include "mfl_tier2.h"
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/* file_read_into_buffer */

/* "(path, buffer, size): RwStreamOpen, RwStreamRead, RwStreamClose" */

/* TIER2: unvalidated 2026-10-19 authority=static */
undefined4 __cdecl file_read_into_buffer(void *path, void *buffer, RwUInt32 length)
{
  RwStream *stream;

  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,path);
  if (stream != (RwStream *)0x0) {
    RwStreamRead(stream,buffer,length);
    RwStreamClose(stream,(void *)0x0);
    return 1;
  }
  return 0;
}
