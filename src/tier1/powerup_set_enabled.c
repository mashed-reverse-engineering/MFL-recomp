#include "mfl_tier2.h"
/*006a8bc0*/ extern struct Powerup g_powerups[25];
#define POWERUP_COUNT 25
#define POWERUP_DISABLED 3
#define POWERUP_AVAILABLE 1

/* powerup_set_enabled(index, enabled): disabling parks the powerup in state 3, enabling a disabled one makes it available again. Returns 1 for a valid index. */
int powerup_set_enabled(int index, int enabled)
{
  struct Powerup *p;

  if (index < 0 || index >= POWERUP_COUNT) {
    return 0;
  }
  p = &g_powerups[index];
  if (enabled != 0) {
    if (p->state == POWERUP_DISABLED) {
      p->state = POWERUP_AVAILABLE;
    }
  } else {
    p->state = POWERUP_DISABLED;
  }
  p->respawn_timer = 0.0f;
  return 1;
}
