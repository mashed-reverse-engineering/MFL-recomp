#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*004c3bf0*/ extern int FUN_004c3bf0(void *p0, undefined4 p1, undefined4 p2);
#define MAP_FILENAME_BUFFER_SIZE (256)
/* load_map_model */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* loads map_%d.dff */
/*
 * Builds the file name "map_<map_id>.dff" and hands it to FUN_004c3bf0
 * (inferred: the map model loader; the two trailing arguments are 0, 0).
 * map_id: map number, printed with %d into the file name.
 * Returns the loader's result (cup_scene_load_models 0x402f62 stores it; VOID_w16: was void, v143 left garbage).
 */
int __cdecl load_map_model(undefined4 map_id)
{
  undefined1 filename [MAP_FILENAME_BUFFER_SIZE];

  sprintf(filename,"map_%d.dff",map_id);
  return FUN_004c3bf0(filename,0,0);
}
