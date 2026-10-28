#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0048e650*/ extern int rwp_object_set_in_world();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00492d10*/ extern int rwp_object_set_active();
/*004c48b0*/ extern void hanim_keyframes_sample_at_time(void *p0, undefined4 p1, undefined4 p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0060d8b0*/ extern undefined g_course_60d8b0;
/*006a188c*/ extern undefined4 g_course_6a188c[0x1000];
/*006a1890*/ extern float g_course_physics_6a1890;
/*006a18b8*/ extern float g_data_6a18b8;
/* course_neustein_cb1_sub_4548b0 */

/* single caller course_neustein_cb1_sub_454970 */

/* Sets two random-jittered globals (offset_a/offset_b +- 0.3) and re-places the object pairs of the table at g_course_6a188c: activates both rwp objects, sets matrices from hanim_keyframes_sample_at_time, the second translated by g_course_60d8b0 (inferred). */
void course_neustein_cb1_sub_4548b0(float offset_a,float offset_b)

{
  int *slot;
  double jitter;
  RwMatrix slot_matrix;
  
  jitter = (double)random_range(0xbe99999a,0x3e99999a);
  g_course_physics_6a1890 = (float)(jitter + (double)offset_a);
  jitter = (double)random_range(0xbe99999a,0x3e99999a);
  slot = g_course_6a188c;
  g_data_6a18b8 = (float)(jitter + (double)offset_b);
  do {
    rwp_object_set_in_world(slot[-1],1);
    rwp_object_set_active(slot[-1],1);
    rwp_object_set_in_world(*slot,1);
    rwp_object_set_active(*slot,1);
    hanim_keyframes_sample_at_time(&slot_matrix,slot[-2],slot[1]);
    rwp_object_set_matrix(*slot,&slot_matrix);
    RwMatrixTranslate(&slot_matrix,(RwV3d *)&g_course_60d8b0,rwCOMBINEPOSTCONCAT);
    rwp_object_set_matrix(slot[-1],&slot_matrix);
    slot = slot + 10;
  } while ((char *)slot < (char *)&g_data_6a18b8 + 0x24);
  return;
}


