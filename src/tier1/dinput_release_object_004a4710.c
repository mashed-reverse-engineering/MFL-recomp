#include "mfl_tier2.h"
/*004a46d0*/ extern int dinput_release_object(void);
/* Duplicate entry point that tail-calls dinput_release_object. */
int dinput_release_object_004a4710(void)
{
  return dinput_release_object();
}
