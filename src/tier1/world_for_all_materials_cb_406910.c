#include "mfl_tier2.h"
extern char LAB_00406910[];
/*004f5dd0*/ extern struct RpWorld * RpWorldForAllMaterials(struct RpWorld *p0, RpMaterialCallBack p1, void *p2);
/* world_for_all_materials_cb_406910 */
/* "RpWorldForAllMaterials(world, FUN_00406910, data)" */
/* Applies the material callback LAB_00406910 (FUN_00406910) to every material in
   `world` via RpWorldForAllMaterials. `user_data` is passed through untouched to
   each callback invocation. Returns nothing. */
void world_for_all_materials_cb_406910(RpWorld *world,void *user_data)
{
  RpWorldForAllMaterials(world,(RpMaterialCallBack)&LAB_00406910,user_data);
  return;
}
