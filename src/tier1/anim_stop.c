#include "mfl_tier2.h"

#define ANIM_OFFSET_STATE 0x48
#define ANIM_OFFSET_CALLBACK 0xc4
#define ANIM_STATE_STOPPED 0
#define ANIM_CALLBACK_STOP 0

/* Stop the animation by clearing its state and invoking the registered callback. */
void anim_stop(char *anim)
{
  /* Reset animation state to stopped */
  *(undefined4 *)(anim + ANIM_OFFSET_STATE) = ANIM_STATE_STOPPED;
  /* Invoke callback if present */
  if (*(code **)(anim + ANIM_OFFSET_CALLBACK) != (code *)0x0) {
    (**(code **)(anim + ANIM_OFFSET_CALLBACK))(anim, ANIM_CALLBACK_STOP);
  }
  return;
}

