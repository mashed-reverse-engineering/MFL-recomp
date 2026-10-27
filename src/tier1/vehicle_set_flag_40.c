#include "mfl_tier2.h"
/*00659f00*/ extern struct VehicleRecord g_vehicle_table[4];
/* vehicle_set_flag_40 */
/* (vehicle_index, flag_40_on, flag_80_on, flag_800_on): sets or clears the 0x40, 0x80 and
   0x800 bits in g_vehicle_table[vehicle_index].flags (+0x294), non-zero argument = set.
   Then applies the cleanup rules (inferred, meaning of the bits not in dossier): 0x400 set
   clears 0x40, 0x80 and 0x800; 0x10 (front) set clears 0x40; 0x20 (back) set clears 0x80
   and 0x800. Returns nothing. */
void vehicle_set_flag_40(int vehicle_index,int flag_40_on,int flag_80_on,int flag_800_on)
{
  struct VehicleRecord *v = &g_vehicle_table[vehicle_index];

  if (flag_40_on != 0) {
    v->flags |= VEHICLE_FLAG_40;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_40;
  }
  if (flag_80_on != 0) {
    v->flags |= VEHICLE_FLAG_80;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_80;
  }
  if (flag_800_on != 0) {
    v->flags |= VEHICLE_FLAG_800;
  }
  else {
    v->flags &= ~VEHICLE_FLAG_800;
  }
  if ((v->flags & VEHICLE_FLAG_400) != 0) {
    v->flags &= 0xfffff73f;
  }
  if ((v->flags & VEHICLE_FLAG_FRONT) != 0) {
    v->flags &= ~VEHICLE_FLAG_40;
  }
  if ((v->flags & VEHICLE_FLAG_BACK) != 0) {
    v->flags &= 0xfffff77f;
  }
  return;
}
