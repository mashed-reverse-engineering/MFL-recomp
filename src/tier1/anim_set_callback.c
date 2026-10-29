#include "mfl_tier2.h"

#define ANIM_CALLBACK_OFFSET 0xc4
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* anim_set_callback */

/* Sets the callback for an animation object (unknown structure). */
/* "stores the callback at +0xC4" */

void anim_set_callback(char *anim, undefined4 callback)
{
  /* Store callback pointer at offset 0xC4 in animation object */
  *(undefined4 *)(anim + ANIM_CALLBACK_OFFSET) = callback;
  return;
}
