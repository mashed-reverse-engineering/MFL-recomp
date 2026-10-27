#include "mfl_tier2.h"
extern char g_course_lap_data_lap_lines_0_[];


/* course_token_list_base */

/* "returns the base pointer of the course token list (used by the course_token_list_* helpers)" */

undefined4 course_token_list_base(void)
{
  return ((int)&g_course_lap_data_lap_lines_0_);
}
