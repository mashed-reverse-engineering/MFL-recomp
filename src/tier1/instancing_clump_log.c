#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*00668c18*/ extern struct CourseWorld g_course_world;
extern RpAtomic * atomic_log_not_instancing(RpAtomic *atomic,int *param_2);


/* instancing_clump_log */

/*  */

void instancing_clump_log(void)
{
  int iVar1;
  undefined4 local_4;
  
  local_4 = 0;
  iVar1 = 0;
  do {
    if (g_course_world.clumps[iVar1] != 0) {
      FID_conflict__wprintf("\ninstancing clump %d\n",iVar1);
      local_4 = 0;
      RpClumpForAllAtomics
                ((RpClump *)g_course_world.clumps[iVar1],atomic_log_not_instancing,&local_4);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x80);
  return;
}
