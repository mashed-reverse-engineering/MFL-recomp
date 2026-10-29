#include "mfl_tier2.h"
/*004dbe70*/ extern RwUInt32 RwStreamRead(struct RwStream *p0, void *p1, RwUInt32 p2);
/*004dc2a0*/ extern RwBool __cdecl RwStreamClose(struct RwStream *p0, void *p1);
/*004dc370*/ extern struct RwStream * RwStreamOpen(enum RwStreamType p0, enum RwStreamAccessType p1, void *p2);
/*0068b9f8*/ extern undefined g_dat_68b9f8;
/*0069ba20*/ extern undefined4 g_dat_69ba20;
/*007ef744*/ extern undefined4 g_game_7ef744;
/* dat_load_language */

/* "english.dat french.dat German.dat Spanish.dat (the DAT string tables)"; g_game_7ef744 = language 0..5 selects english, french, German, Spanish, Italian or USA.dat, read as one 0x10000 byte block; returns 1 on success, 0 on an unknown language or open failure */

undefined4
dat_load_language(void)
{
  RwStream *stream;
  RwStream *pRVar1;
  undefined *puVar2;
  char *pData;
  undefined4 uVar3;
  
  switch(g_game_7ef744) {
  case 0:
    pData = "english.dat";
    break;
  case 1:
    pData = "french.dat";
    break;
  case 2:
    pData = "German.dat";
    break;
  case 3:
    pData = "Spanish.dat";
    break;
  case 4:
    pData = "Italian.dat";
    break;
  case 5:
    pData = "USA.dat";
    break;
  default:
    goto switchD_0042ba2b_default;
  }
  stream = RwStreamOpen(rwSTREAMFILENAME,rwSTREAMREAD,pData);
  if (stream != (RwStream *)0x0) {
    uVar3 = 0x10000;
    puVar2 = &g_dat_68b9f8;
    pRVar1 = stream;
    RwStreamRead(stream,&g_dat_68b9f8,0x10000);
    RwStreamClose(stream,(void *)0x0);
    g_dat_69ba20 = 1;
    return 1;
  }
switchD_0042ba2b_default:
  return 0;
}


