#include "mfl_tier2.h"
/*004d30f0*/ extern RwBool RwEngineStop();
/*004d31d0*/ extern RwBool __cdecl RwEngineClose(void);
/*004d3400*/ extern RwBool RwEngineTerm();
/*007daaa0*/ extern undefined4 g_data_7daaa0;
/* gap_5b6560 */

/* no context */

void gap_5b6560(void)

{
  if (g_data_7daaa0 == 0) {
    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();
    return;
  }
  return;
}
