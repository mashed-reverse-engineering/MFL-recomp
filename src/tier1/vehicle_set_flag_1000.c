#include "mfl_tier2.h"
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* vehicle_set_flag_1000 */
/* "(vehicle, on): flag 0x1000" */
/* (vehicle_index, flag_1000_on, flag_2000_on): sets or clears VEHICLE_FLAG_1000 (0x1000)
   in g_vehicle_table[vehicle_index].flags (+0x294). flag_2000_on non-zero also sets
   VEHICLE_FLAG_2000 (0x2000), zero clears it (inferred: meaning of 0x1000 / 0x2000 not in
   dossier). Parameters: vehicle_index = slot 0..3, flag_*_on = non-zero to set, zero to clear.
   Returns nothing. */
void vehicle_set_flag_1000(int vehicle_index,int flag_1000_on,int flag_2000_on)
{
  struct VehicleRecord *v = &g_vehicle_table[vehicle_index];

  if (flag_1000_on != 0) {
    v->flags |= VEHICLE_FLAG_1000;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_1000;
  }
  if (flag_2000_on != 0) {
    v->flags |= VEHICLE_FLAG_2000;
    return;
  }
  v->flags &= ~VEHICLE_FLAG_2000;
  return;
}
