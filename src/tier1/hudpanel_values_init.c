#include "mfl_tier2.h"
/*00658a28*/ extern undefined4 g_particles_658a28;
/*00658a28*/ extern undefined4 g_particles_658a28;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* hudpanel_values_init */

/* "fills the 0x40 float values at 0x658A28 with 1.75" */

void hudpanel_values_init(void)
{
  int words_left;
  undefined4 *dst;
  
  dst = &g_particles_658a28;
  for (words_left = 0x40; words_left != 0; words_left = words_left + -1) {
    *dst = 0x3fe00000;
    dst = dst + 1;
  }
  return;
}
