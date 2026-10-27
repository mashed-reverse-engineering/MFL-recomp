#include "mfl_tier2.h"
/*00424220*/ extern void car_damage_smoke_destroy(void);
/* Duplicate entry point that tail-calls car_damage_smoke_destroy. */
void car_damage_smoke_destroy_004246c0(void)
{
  car_damage_smoke_destroy();
}
