#include "mfl_tier2.h"
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*004dc8d0*/ extern struct RwStream * RwStreamReadInt32(struct RwStream *p0, RwInt32 *p1, RwUInt32 p2);
/*004df060*/ extern RwMatrix * RwMatrixStreamRead(struct RwStream *p0, RwMatrix *p1);
/* matrix_list_load */

/* "reads count and matrices (chunk 0xD) from a file into an array" */

uint matrix_list_load(void *pData,RwMatrix *matrix)

{
  uint count;
  RwUInt32 chunk_length;
  RwUInt32 chunk_type;
  RwStream *stream;
  uint index;
  
  chunk_length = 0;
  chunk_type = 0;
  count = 0;
  if ((pData != (void *)0x0) &&
     (stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,pData), stream != (RwStream *)0x0)) {
    RwStreamReadInt32(stream,(RwInt32 *)&count,4);
    for (index = 0; index < count; index = index + 1) {
      RwStreamFindChunk(stream,0xd,&chunk_type,&chunk_length);
      RwMatrixStreamRead(stream,matrix);
      matrix = matrix + 1;
    }
    RwStreamClose(stream,(void *)0x0);
    return count;
  }
  return 0;
}
