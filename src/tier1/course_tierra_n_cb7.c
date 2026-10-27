#include "mfl_tier2.h"
/*004577b0*/ extern void SUB_004577b0(void);
/*00457520*/ extern void LAB_00457520(void);
/* course_tierra_n_cb7 */

/* Course Tierra-N callback: runs SUB_004577b0, then the shared routine (LAB_00457520). */
void course_tierra_n_cb7(void)

{
  SUB_004577b0();
  LAB_00457520();
  return;
}
