#include "mfl_tier2.h"
/*004a0660*/ extern int rain_drops_update(void);

int
rain_drops_update_and_respawn(void)
{
  return rain_drops_update();
}
