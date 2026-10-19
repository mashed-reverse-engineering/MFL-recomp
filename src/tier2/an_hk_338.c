#include "mfl_tier2.h"
/*004d30f0*/ extern RwBool RwEngineStop();
/*004d31d0*/ extern RwBool __cdecl RwEngineClose(void);
/*004d3400*/ extern RwBool RwEngineTerm();
/*005219a6*/ extern int Close();
/*005609c0*/ extern void FUN_005609c0(void);
/* rw_engine_stop_close_term */

/* Shuts the RenderWare engine down. Takes no parameters and returns nothing.
 * Order matters: RtCharsetClose() and FUN_005609c0() (inferred: game-side cleanup)
 * run first, then RwEngineStop(), RwEngineClose() and RwEngineTerm().
 * "RwEngineStop/Close/Term after two helper calls" */

void rw_engine_stop_close_term(void)

{
  RtCharsetClose();
  FUN_005609c0();
  RwEngineStop();
  RwEngineClose();
  RwEngineTerm();
  return;
}


