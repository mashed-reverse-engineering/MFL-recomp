#include "mfl_tier2.h"
/*006aaf74*/ extern undefined4 g_weapon_6aaf74;


/* sound_tick_counter_increment */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_weapon_6aaf74 += 1" */

void sound_tick_counter_increment(void)
{
  g_weapon_6aaf74 = g_weapon_6aaf74 + 1;
  return;
}
