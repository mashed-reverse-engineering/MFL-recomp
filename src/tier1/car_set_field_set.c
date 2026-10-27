#include "mfl_tier2.h"
/*008b06e0*/ extern struct Car player_car[16];
#define MAX_CARS (16)
#define EFFECT_LEVEL_MAX (100)
/* car_set_field_set */
/* Sets the effect level and hold time of one effect slot of a car.
 * car_index: index into player_car, must be below MAX_CARS.
 * effect_kind: 1..6, selects slot effect_kind - 1 in effect_level[] and effect_hold[].
 * level: effect level, capped at EFFECT_LEVEL_MAX (0..100 range, see the Car struct).
 * hold_ms: ms the level is held before it decays (see cars_update_timers).
 * Returns 1 when the slot was written, 0 when car_index or effect_kind is out of range.
 * (was: "(car, kind 1..3, value<=100, extra): stores into field_0x20/0x24/0x28 and 0x38/0x3c/0x40") */
undefined4 car_set_field_set(uint car_index,undefined4 effect_kind,uint level,float hold_ms)
{
  if (car_index < MAX_CARS) {
    /* cap the level at the maximum before storing it */
    if (EFFECT_LEVEL_MAX < level) {
      level = EFFECT_LEVEL_MAX;
    }
    /* one case per effect kind: store the level and the hold time into that slot */
    switch(effect_kind) {
    case 1:
      player_car[car_index].effect_level[0] = level;
      player_car[car_index].effect_hold[0] = hold_ms;
      return 1;
    case 2:
      player_car[car_index].effect_level[1] = level;
      player_car[car_index].effect_hold[1] = hold_ms;
      return 1;
    case 3:
      player_car[car_index].effect_level[2] = level;
      player_car[car_index].effect_hold[2] = hold_ms;
      return 1;
    case 4:
      player_car[car_index].effect_level[3] = level;
      player_car[car_index].effect_hold[3] = hold_ms;
      return 1;
    case 5:
      player_car[car_index].effect_level[4] = level;
      player_car[car_index].effect_hold[4] = hold_ms;
      return 1;
    case 6:
      player_car[car_index].effect_level[5] = level;
      player_car[car_index].effect_hold[5] = hold_ms;
      return 1;
    }
  }
  return 0;
}
