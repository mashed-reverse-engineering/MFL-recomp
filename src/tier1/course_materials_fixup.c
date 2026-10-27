#include "mfl_tier2.h"
extern char LAB_004065c0[];
extern char LAB_00406690[];
/*004065a0*/ extern int object_set_pipeline_7c_forward();
/*004f5dd0*/ extern struct RpWorld * RpWorldForAllMaterials(struct RpWorld *p0, RpMaterialCallBack p1, void *p2);
/*004f5e10*/ extern struct RpWorld * RpWorldForAllWorldSectors(struct RpWorld *p0, RpWorldSectorCallBack p1, void *p2);
/* course_materials_fixup */
/* Fixes up a course world's materials and sectors (inferred).
 * world: the RpWorld to process. callback_data: passed to the second material callback (0x406690).
 * Returns nothing; the world is modified in place.
 * Uses RpWorldForAllMaterials / RpWorldForAllWorldSectors with the callbacks at 0x4065C0 / 0x406690. */
void __cdecl course_materials_fixup(RpWorld *world,void *callback_data)
{
  int needs_fixup;

  /* Pass 1: the first material callback (0x4065C0) sets needs_fixup if any material needs work (inferred). */
  needs_fixup = 0;
  RpWorldForAllMaterials(world,(RpMaterialCallBack)&LAB_004065c0,&needs_fixup);
  /* Pass 2, only if pass 1 found something: set the sector pipeline (inferred), then run the second material callback. */
  if (needs_fixup != 0) {
    RpWorldForAllWorldSectors(world,object_set_pipeline_7c_forward,(void *)0x0);
    RpWorldForAllMaterials(world,(RpMaterialCallBack)&LAB_00406690,callback_data);
  }
  return;
}
