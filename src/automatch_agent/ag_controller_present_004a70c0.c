#include "mfl_tier2.h"
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];


/* controller_present_004a70c0 */

int __cdecl controller_present_004a70c0(int controller_index)

{
  if (3 < controller_index) {
    return 0;
  }
  return g_controller_cfg[controller_index].device != 0;
}
