#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*00668c18*/ extern struct CourseWorld g_course_world;
extern RpAtomic * atomic_log_not_instancing(RpAtomic *atomic,int *param_2);


/* instancing_clump_log */

/*  */

/* Debug: logs, for each of the 128 course clump slots, the atomics that are not instancing. */
void instancing_clump_log(void)
{
  int clump_index;
  undefined4 not_instancing_count;
  
  not_instancing_count = 0;
  clump_index = 0;
  do {
    if (g_course_world.clumps[clump_index] != 0) {
      FID_conflict__wprintf("\ninstancing clump %d\n",clump_index);
      not_instancing_count = 0;
      RpClumpForAllAtomics
                ((RpClump *)g_course_world.clumps[clump_index],atomic_log_not_instancing,&not_instancing_count);
    }
    clump_index = clump_index + 1;
  } while (clump_index < 0x80);
  return;
}
