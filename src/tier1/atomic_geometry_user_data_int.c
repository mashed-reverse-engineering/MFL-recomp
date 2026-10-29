#include "mfl_tier2.h"
/*005542c0*/ extern int rp_geometry_user_data_filled_count(undefined4 p0);
/*005542f0*/ extern int rp_geometry_user_data_array_get(undefined4 p0, undefined4 p1);
/*005543e0*/ extern int rw_second_field(int * p0);
/*005543f0*/ extern int __cdecl rp_user_data_array_get_int(int a1, int a2);
#define ATOMIC_GEOMETRY_OFFSET (0x18)
/* atomic_geometry_user_data_int */
/*
 * (inferred) RpUserData lookup on the atomic's geometry: returns the int stored at
 * element `element_index` of user-data array `array_index` (callers pass 0,0: the atomic id).
 * The two intermediate calls (array count, second field) have unused results but are kept.
 */
int atomic_geometry_user_data_int(char *atomic,int array_index,int element_index)
{
  undefined4 geometry;
  int array;

  geometry = *(undefined4 *)(atomic + ATOMIC_GEOMETRY_OFFSET);
  rp_geometry_user_data_filled_count(geometry);
  array = rp_geometry_user_data_array_get(geometry,array_index);
  rw_second_field(array);
  return rp_user_data_array_get_int(array,element_index);
}
