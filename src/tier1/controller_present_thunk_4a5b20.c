#include "mfl_tier2.h"
/*004a70c0*/ extern int controller_present_004a70c0(int controller_index);

/* Jump stub at 0x4a5b20: forwards to controller_present_004a70c0 (cdecl, one stack argument). */
int
controller_present_thunk_4a5b20(int controller_index)
{
  return controller_present_004a70c0(controller_index);
}
