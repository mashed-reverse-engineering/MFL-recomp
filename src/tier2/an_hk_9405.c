#include "mfl_tier2.h"
/*00426ff0*/ extern int ai_file_name();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*004dc720*/ extern RwBool RwStreamFindChunk(struct RwStream *p0, RwUInt32 p1, RwUInt32 *p2, RwUInt32 *p3);
/*00664320*/ extern undefined1 g_ai_file_name;
/*007f0284*/ extern struct AiData g_ai_data;
/* ai_load */

/* "opens ai.piz, reads chunk 0x13269902 (0x11884 bytes, struct AiData) into g_ai_data" */

undefined4
ai_load(void)
{
  undefined4 pu_stk8;
  RwUInt32 pu_stk4;
  undefined4 pu_stk0;
  RwStream *stream;
  RwBool RVar1;
  undefined4 uVar2;
  RwStream *pRVar3;
  AiData *pAVar4;
  RwUInt32 RVar5;
  
  uVar2 = 0;
  ai_file_name();
  game_open_piz("d:\\ToastArt\\Common\\ai.piz");
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,&g_ai_file_name);
  if (stream != (RwStream *)0x0) {
    RVar1 = RwStreamFindChunk(stream,0x13269902,(RwUInt32 *)&pu_stk4,
                              (RwUInt32 *)&pu_stk0);
    if (RVar1 != 0) {
      pAVar4 = &g_ai_data;
      pRVar3 = stream;
      RVar5 = pu_stk4;
      RwStreamRead(stream,&g_ai_data,pu_stk4);
      RwStreamClose(stream,(void *)0x0);
      uVar2 = 1;
    }
  }
  game_close_piz();
  return uVar2;
}


