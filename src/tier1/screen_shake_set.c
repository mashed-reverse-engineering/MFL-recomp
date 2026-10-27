#include "mfl_tier2.h"
/*008c8720*/ extern undefined4 g_hit_shake;


/* screen_shake_set */

/* Sets the screen shake amount. */
/* "sets g_hit_shake (inferred name)" */

void screen_shake_set(undefined4 shake)
{
  g_hit_shake = shake;
  return;
}
