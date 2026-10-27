#include "mfl_tier2.h"
/*00459810*/ extern void course_object_offsets_init(void);
/* Duplicate entry point that tail-calls course_object_offsets_init. */
void course_object_offsets_init_b(void)
{
  course_object_offsets_init();
}
