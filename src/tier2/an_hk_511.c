#include "mfl_tier2.h"
extern char UNK_004575c0[];
extern char UNK_005df57c[];
/*0050b5e0*/ extern int nop_stub();
/* course_rouabout_cb12 */
/* Course setup for the Roundabout track (inferred from the course name).
 * Takes no parameters and returns nothing.
 * Calls func_0x004801f0 twice (callee name not known yet):
 *   1. with the "RoundaboutStartCrane" string and UNK_004575c0 (inferred: a named asset/data block)
 *   2. with UNK_005df57c and nop_stub as a function pointer (inferred: callback; nop_stub is an empty stub)
 */

void course_rouabout_cb12(void)

{
  func_0x004801f0("RoundaboutStartCrane",&UNK_004575c0);
  func_0x004801f0(&UNK_005df57c,nop_stub);
  return;
}


