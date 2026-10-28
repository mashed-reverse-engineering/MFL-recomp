#include "mfl_tier2.h"
/*0048e650*/ extern int rwp_object_set_in_world();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004c48b0*/ extern void hanim_keyframes_sample_at_time(void *p0, undefined4 p1, undefined4 p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0060d8b0*/ extern undefined g_course_60d8b0;
/*006a188c*/ extern undefined4 g_course_6a188c;
/*006a18dc*/ extern char g_course_physics_6a18dc[];

/* ints per physics pair in the course table (inferred from the loop step) */
#define PHYSICS_PAIR_STRIDE (10)

/* course_physics_objects_reset */

/* "resets the 10 physics object pairs of the course (positions, rwp flags)" */

void course_physics_objects_reset(void)

{
  int *pair;
  RwMatrix pair_matrix;

  pair = &g_course_6a188c;
  /* for each pair: put both rwp objects back in the world, clear the
     slot at pair[1] (inferred: reset flag), place the second object at the
     transform from pair[-2] (inferred: source), then move the first object
     by g_course_60d8b0 (inferred: offset) */
  do {
    rwp_object_set_in_world(pair[-1],1);
    rwp_object_set_in_world(*pair,1);
    pair[1] = 0;
    hanim_keyframes_sample_at_time(&pair_matrix,pair[-2],0);
    rwp_object_set_matrix(*pair,&pair_matrix);
    RwMatrixTranslate(&pair_matrix,(RwV3d *)&g_course_60d8b0,rwCOMBINEPOSTCONCAT);
    rwp_object_set_matrix(pair[-1],&pair_matrix);
    pair = pair + PHYSICS_PAIR_STRIDE;
  } while ((LONG_PTR)pair < ((LONG_PTR)&g_course_physics_6a18dc));
  return;
}


