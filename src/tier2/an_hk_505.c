#include "mfl_tier2.h"
/* course_dump_cb1
 * Course dump callback (inferred from name): forwards its single argument
 * unchanged to func_0x00453600, which does the actual work.
 * callback_arg: the value passed through to func_0x00453600 (meaning unknown).
 */

void course_dump_cb1(undefined4 callback_arg)

{
  func_0x00453600(callback_arg);
  return;
}


