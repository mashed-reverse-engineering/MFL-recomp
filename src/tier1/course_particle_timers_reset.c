#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*0060d8a4*/ extern float g_course_particle_60d8a4;
/*006a1418*/ extern undefined4 g_course_particle_6a1418;


/* course_particle_timers_reset */

/* Resets every particle timer to (g_course_particle_60d8a4 + 1.0). */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills the 0x100 particle timers at 0x6A1418 with limit + 1.0" */

#define PARTICLE_TIMER_COUNT 0x100

void course_particle_timers_reset(void)
{
  float timer_value;
  int remaining;
  float *timer_ptr;

  timer_value = g_course_particle_60d8a4 + float_1_0;
  timer_ptr = (float *)&g_course_particle_6a1418;
  for (remaining = PARTICLE_TIMER_COUNT; remaining != 0; remaining = remaining + -1) {
    *timer_ptr = timer_value;
    timer_ptr = timer_ptr + 1;
  }
  return;
}
