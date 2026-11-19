#include "mfl_tier2.h"
/*0041da00*/ extern int btb_panel_destroy_extra();
/*00421d50*/ extern int vehicle_prepare_parts();
/*00422400*/ extern int __cdecl vehicle_get_active_flag(undefined4 p0);
/*004254c0*/ extern int debris_any_active_all_vehicles();
/*0050b5e0*/ extern int nop_stub();


/* vehicles_any_active */

/* "true when any vehicle visual record is active" */

void vehicles_any_active(void)

{
  int any_active;
  int active_flag;
  int vehicle_index;
  
  any_active = 0;
  vehicle_index = 0;
  do {
    active_flag = vehicle_get_active_flag(vehicle_index);
    if (active_flag != 0) {
      any_active = 1;
    }
    vehicle_index = vehicle_index + 1;
  } while (vehicle_index < 4);
  if (any_active) {
    btb_panel_destroy_extra();
    debris_any_active_all_vehicles();
    vehicle_index = 0;
    do {
      vehicle_prepare_parts(vehicle_index);
      vehicle_index = vehicle_index + 1;
    } while (vehicle_index < 4);
    nop_stub();
    return;
  }
  return;
}
