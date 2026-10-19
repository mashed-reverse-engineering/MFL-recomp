#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0065a9f4*/ extern undefined4 g_car_damage_65a9f4;
/*0065aa94*/ extern char g_car_damage_65aa94[];
#define DAMAGE_EVENT_SIZE (8)
#define DAMAGE_EVENT_STRIDE_WORDS (10)
/* car_damage_events_reset */

/* "clears the 8 byte records of the damage event table at 0x65A9F4"
 * Takes no parameters and returns nothing. Walks the table from
 * g_car_damage_65a9f4 up to g_car_damage_65aa94 (end of table), one entry
 * at a time, and zeroes each entry. (inferred: entry layout from the stores)
 */

void car_damage_events_reset(void)

{
  undefined4 *event_cursor;

  event_cursor = &g_car_damage_65a9f4;
  /* zero each entry: 8 bytes via mem_zero at cursor-3 words, then the
     words at cursor-1, cursor, cursor+1 and cursor+2; advance by one stride */
  do {
    mem_zero(event_cursor + -3,DAMAGE_EVENT_SIZE);
    event_cursor[-1] = 0;
    *event_cursor = 0;
    event_cursor[1] = 0;
    event_cursor[2] = 0;
    event_cursor = event_cursor + DAMAGE_EVENT_STRIDE_WORDS;
  } while ((LONG_PTR)event_cursor < ((LONG_PTR)&g_car_damage_65aa94));
  return;
}


