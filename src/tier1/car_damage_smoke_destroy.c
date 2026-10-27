#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*0065aa88*/ extern undefined g_car_damage_65aa88;


/* car_damage_smoke_destroy */

/* "destroys the damage smoke particle system" */

void car_damage_smoke_destroy(void)
{
  particles_destroy(&g_car_damage_65aa88);
  return;
}
