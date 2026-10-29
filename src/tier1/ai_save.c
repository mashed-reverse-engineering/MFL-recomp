#include "mfl_tier2.h"
/*00426ff0*/ extern int ai_file_name();
/*004dbfc0*/ extern struct RwStream * RwStreamWrite(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc6c0*/ extern struct RwStream * _rwStreamWriteVersionedChunkHeader(struct RwStream *p0, RwInt32 p1, RwInt32 p2, RwUInt32 p3, RwUInt32 p4);
/*00664320*/ extern undefined1 g_ai_file_name;
/*007f0284*/ extern struct AiData g_ai_data;
/* ai_save */

/* "writes chunk 0x13269902 with the 0x11884 byte g_ai_data to AI<course id>.AI (editor feature)" */

undefined4
ai_save(void)
{
  RwStream *stream;
  RwStream *pRVar1;
  
  ai_file_name();
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMWRITE,&g_ai_file_name);
  if (stream != (RwStream *)0x0) {
    _rwStreamWriteVersionedChunkHeader(stream,0x13269902,0x11884,0x37002,0x24);
    pRVar1 = RwStreamWrite(stream,&g_ai_data,0x11884);
    if (pRVar1 != (RwStream *)0x0) {
      RwStreamClose(stream,(void *)0x0);
      return 1;
    }
  }
  return 0;
}
