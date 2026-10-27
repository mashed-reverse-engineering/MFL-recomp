#include "mfl_tier2.h"

#define VEHICLE_DEBRIS_RECORD_SIZE 0x208
#define VEHICLE_DEBRIS_ARRAY_END ((int)&g_game_65c8f0)

/*00424db0*/ extern int debris_vehicle_destroy_all();
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*0065c8f0*/ extern char g_game_65c8f0[];


/* debris_any_active_all_vehicles */

/* Destroys all vehicle debris across all vehicles. */
/* "loops vehicle debris records calling vehicles_any_active_sub_424db0" */

void debris_any_active_all_vehicles(void)
{
  undefined *vehicle_debris;
  
  vehicle_debris = &g_game_65c0d0;
  /* Iterate over debris records for all 4 vehicles */
  do {
    debris_vehicle_destroy_all(vehicle_debris);
    vehicle_debris = vehicle_debris + VEHICLE_DEBRIS_RECORD_SIZE;
  } while ((int)vehicle_debris < VEHICLE_DEBRIS_ARRAY_END);
  return;
}
