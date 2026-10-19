#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0065a9e8*/ extern undefined4 car_damage;
/*0065aa00*/ extern undefined4 g_car_damage_65aa00;
/*0065aa28*/ extern undefined4 g_car_damage_65aa28;
/*0065aa50*/ extern undefined4 g_car_damage_65aa50;
/*0065aa78*/ extern undefined4 g_car_damage_65aa78;

#define CAR_DAMAGE_TABLE_SIZE (0xa0)

/* car_damage_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the 0xA0 byte car_damage table and sets the default per car ids (0, 1, 2 ...)" */
/* Resets the car damage table at startup: zeroes CAR_DAMAGE_TABLE_SIZE bytes, then stores
   the default car ids 0..3 into the four globals, which sit inside the table at
   offsets 0x18, 0x40, 0x68, 0x90 (4 entries of 0x28 bytes each, inferred).
   Takes no parameters and returns nothing. */

void car_damage_init(void)

{
  mem_zero(&car_damage,CAR_DAMAGE_TABLE_SIZE);
  g_car_damage_65aa00 = 0;
  g_car_damage_65aa28 = 1;
  g_car_damage_65aa50 = 2;
  g_car_damage_65aa78 = 3;
  return;
}


