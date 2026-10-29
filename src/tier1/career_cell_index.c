#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*00435f10*/ extern int get_career_row();
/* number of career cells per career row, one per event type (inferred) */
#define CAREER_CELLS_PER_ROW (3)
/* career_cell_index */
/* "(row * 3) + type offset (type 4: +1, 5: +2)" */
/* Returns the career cell index of the current event: the current career row times
   CAREER_CELLS_PER_ROW, plus an offset of 1 for event type 4 and 2 for event type 5
   (other types use the row base). Takes no parameters. */
int career_cell_index(void)
{
  int career_row;
  int event_type;
  int event_offset;

  /* row base: the career row of the current event, scaled to its first cell */
  career_row = get_career_row();
  career_row = career_row * CAREER_CELLS_PER_ROW;
  event_type = get_event_type();
  event_offset = 0;
  if (event_type == 4) {
    event_offset = 1;
  } else if (event_type == 5) {
    event_offset = 2;
  }
  return event_offset + career_row;
}
