#include "mfl_tier2.h"
/*0069cc40*/ extern undefined4 g_team_mode;


/* is_team_mode */

/* "returns the flag at 0x69CC40 (team play)" */

undefined4 is_team_mode(void)
{
  return g_team_mode;
}
