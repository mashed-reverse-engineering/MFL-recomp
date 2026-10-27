#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0048e650*/ extern int rwp_object_set_in_world();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00492d10*/ extern int rwp_object_set_active();
/*004c48b0*/ extern void FUN_004c48b0(void *p0, undefined4 p1, undefined4 p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0060d8b0*/ extern undefined g_course_60d8b0;
/*006a188c*/ extern undefined4 g_course_6a188c[0x1000];
/*006a1890*/ extern float g_course_physics_6a1890;
/*006a18b8*/ extern float g_data_6a18b8;
/* course_neustein_cb1_sub_4548b0 */

/* single caller course_neustein_cb1_sub_454970 */

void course_neustein_cb1_sub_4548b0(float param_1,float param_2)

{
  int *piVar1;
  double fVar2;
  RwMatrix RStack_40;
  
  fVar2 = (double)random_range(0xbe99999a,0x3e99999a);
  g_course_physics_6a1890 = (float)(fVar2 + (double)param_1);
  fVar2 = (double)random_range(0xbe99999a,0x3e99999a);
  piVar1 = g_course_6a188c;
  g_data_6a18b8 = (float)(fVar2 + (double)param_2);
  do {
    rwp_object_set_in_world(piVar1[-1],1);
    rwp_object_set_active(piVar1[-1],1);
    rwp_object_set_in_world(*piVar1,1);
    rwp_object_set_active(*piVar1,1);
    FUN_004c48b0(&RStack_40,piVar1[-2],piVar1[1]);
    rwp_object_set_matrix(*piVar1,&RStack_40);
    RwMatrixTranslate(&RStack_40,(RwV3d *)&g_course_60d8b0,rwCOMBINEPOSTCONCAT);
    rwp_object_set_matrix(piVar1[-1],&RStack_40);
    piVar1 = piVar1 + 10;
  } while ((char *)piVar1 < (char *)&g_data_6a18b8 + 0x24);
  return;
}


