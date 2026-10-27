#include "mfl_tier2.h"
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
extern int __cdecl geometry_for_all_materials_cb(int param_1,undefined4 *param_2);


/* clump_for_all_materials */

/* "RpClumpForAllAtomics with geometry_for_all_materials_cb" */

RpClump * clump_for_all_materials(RpClump *clump,void *pData)
{
  RpClumpForAllAtomics(clump,geometry_for_all_materials_cb,pData);
  return clump;
}
