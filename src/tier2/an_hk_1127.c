#include "mfl_tier2.h"
/*004d3140*/ extern RwBool __cdecl RwEngineStart(void);
/*004d31d0*/ extern RwBool __cdecl RwEngineClose(void);
/*004d3240*/ extern RwBool RwEngineOpen(struct RwEngineOpenParams *p0);
/*004d3400*/ extern RwBool RwEngineTerm();
/*004d3440*/ extern RwBool __cdecl RwEngineInit(struct RwMemoryFunctions *p0, RwUInt32 p1, RwUInt32 p2);
/*007daa94*/ extern char *g_data_7daa94;
/*007daaa0*/ extern undefined4 g_data_7daaa0;
/* gap_5b6500 */

/* no context */

undefined4 gap_5b6500(void)

{
  RwBool RVar1;
  RwEngineOpenParams RStack_4;
  
  if (g_data_7daaa0 != 0) {
    return 1;
  }
  RVar1 = RwEngineInit((struct RwMemoryFunctions *)g_data_7daa94,0,0x400);
  if (RVar1 != 0) {
    RStack_4.displayID = (void *)0x1;
    RVar1 = RwEngineOpen(&RStack_4);
    if (RVar1 != 0) {
      RVar1 = RwEngineStart();
      if (RVar1 != 0) {
        return 1;
      }
      RwEngineClose();
    }
    RwEngineTerm();
  }
  return 0;
}


