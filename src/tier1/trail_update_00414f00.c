#include "mfl_tier2.h"
/*00414790*/ extern int trail_update(void);
/* Duplicate entry point that tail-calls trail_update. */
int trail_update_00414f00(void)
{
  return trail_update();
}
