#include "mfl_tier2.h"
/*0041c4d0*/ extern int hudpanel_reset_all_b();

/* Resets all HUD panels by delegating to hudpanel_reset_all_b (inferred from the
   callee name; the body is a single call with no parameters and no return value). */
void hudpanel_reset_all(void)

{
  hudpanel_reset_all_b();
}
