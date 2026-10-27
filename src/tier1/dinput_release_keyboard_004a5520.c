#include "mfl_tier2.h"
/*004a54c0*/ extern int dinput_release_keyboard(void);
/* Duplicate entry point that tail-calls dinput_release_keyboard. */
int dinput_release_keyboard_004a5520(void)
{
  return dinput_release_keyboard();
}
