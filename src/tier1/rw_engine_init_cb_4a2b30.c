#include "mfl_tier2.h"
/*0076e864*/ extern code *g_rw_engine_76e864;
/* rw_engine_init_cb_4a2b30 */

/* function pointer pushed/stored by rw_engine_init (PUSH 0x4a2b30) */

void rw_engine_init_cb_4a2b30(void)

{
  if (g_rw_engine_76e864 != (code *)0x0) {
    (*g_rw_engine_76e864)();
    return;
  }
  return;
}
