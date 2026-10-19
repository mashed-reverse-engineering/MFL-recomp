#include "mfl_tier2.h"
/*0042b0d0*/ extern int course_get_safe_start_count();
/*0042b0e0*/ extern int course_get_lap_line();
/* course_lap_line_find_index */

/* "loops course_get_lap_line over the safe start count and returns the matching index" */

/*
 * Finds the lap line entry for the current position.
 * lap_pos (in/out): current lap position; on return it is set to the matching lap line value
 * (inferred). Returns early when a match is found; otherwise it falls back to the value of
 * the first entry.
 */
void course_lap_line_find_index(int *lap_pos)

{
  int safe_start_count;
  short first_lap_short;
  int lap_index;
  struct {
    short line_range [2];
    undefined4 line_value;
  } lap_line;
  undefined4 first_lap_value;

  safe_start_count = course_get_safe_start_count();
  first_lap_value = 0;
  first_lap_short = 0;
  if (safe_start_count != 0) {
    lap_index = 0;
    if (0 < safe_start_count) {
      /* walk every safe start entry; stop early on a match (inferred) */
      do {
        course_get_lap_line(lap_index,&lap_line.line_value,lap_line.line_range);
        if (lap_index == 0) {
          first_lap_value = lap_line.line_value;
        }
        first_lap_short = (short)first_lap_value;
        if (*lap_pos < (int)(short)lap_line.line_value) {
          *lap_pos = (int)(short)lap_line.line_value;
          return;
        }
        if (*lap_pos <= (int)lap_line.line_range[0]) {
          return;
        }
        lap_index = lap_index + 1;
      } while (lap_index < safe_start_count);
    }
    *lap_pos = (int)first_lap_short;
  }
  return;
}


