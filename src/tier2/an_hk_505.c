#include "mfl_tier2.h"
/* course_dump_cb1
 * Course dump callback (inferred from name): forwards its single argument
 * unchanged to course_dump_emitters_update, which does the actual work.
 * callback_arg: the value passed through to course_dump_emitters_update (meaning unknown).
 */

void course_dump_cb1(undefined4 callback_arg)

{
  course_dump_emitters_update(callback_arg);
  return;
}


