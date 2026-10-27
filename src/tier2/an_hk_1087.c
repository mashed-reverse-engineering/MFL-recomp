#include "mfl_tier2.h"
/*0076e868*/ extern code *g_rw_engine_76e868;
/* rw_engine_init_cb_4a2b40 */

/* function pointer pushed/stored by rw_engine_init (PUSH 0x4a2b40) */

void rw_engine_init_cb_4a2b40(void)

{
  if (g_rw_engine_76e868 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x004a2b49. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*g_rw_engine_76e868)();
    return;
  }
  return;
}


