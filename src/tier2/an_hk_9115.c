#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*004f7560*/ extern struct RpClump * RpClumpStreamRead(struct RwStream *p0);
/* clump_load_logged */

/* "loaded clump %s length = %d, version = %x" */

RpClump * clump_load_logged(void *pData,RwChar *path)

{
  RwStream *stream;
  RpClump *clump;
  RwUInt32 length;
  RwUInt32 version;

  clump = (RpClump *)0x0;
  length = 0;
  version = 0;
  if (path != (RwChar *)0x0) {
    RwImageSetPath(path);
  }
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,pData);
  if (stream != (RwStream *)0x0) {
    RwStreamFindChunk(stream,0x10,&length,&version);
    clump = RpClumpStreamRead(stream);
    FID_conflict__wprintf("loaded clump %s length = %d, version = %x\n",pData,length,version);
    RwStreamClose(stream,(void *)0x0);
  }
  return clump;
}
