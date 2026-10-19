#include "mfl_tier2.h"
/*0069cbd4*/ extern uint g_event_type;
/* menu_event_type_switch_b */

/* "returns a value by event type via a jump table" */

uint menu_event_type_switch_b(void)

{
  uint event_type;
  
  event_type = g_event_type;
  if (event_type < 0x12) {
    switch(event_type) {
    case 0:
    case 1:
    case 2:
      return 1;
    case 3:
    case 4:
    case 5:
      return 4;
    case 6:
    case 7:
    case 8:
      return 7;
    case 9:
    case 10:
      return 5;
    case 11:
    case 12:
    case 13:
      return 12;
    case 14:
    case 15:
    case 16:
    case 17:
      return 0;
    }
  }
  return event_type;
}
