#include "mfl_tier2.h"
/*0069cbd4*/ extern undefined4 g_event_type;


/* get_event_type */

/* "returns g_event_type (the column of the career grid, 0..0x11; 0x11 is a special variant)" */

undefined4 get_event_type(void)
{
  return g_event_type;
}
