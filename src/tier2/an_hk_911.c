#include "mfl_tier2.h"
/*0040ad10*/ extern int course_editor_read_chunk();
/*0040ae20*/ extern char *gamesave_ptr_to_line_exceptions();
/*0042b1d0*/ extern char *course_get_record_base();
/*0042b210*/ extern char *course_get_record_point();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/* camera_pick_ground_point */

/* "ray from the camera down (0, -1, 0) for the replay / editor view" */

void __cdecl camera_pick_ground_point(int param_1,RwV3d *param_2,float *param_3)

{
  char *iVar1;
  RwReal *pRVar2;
  float *pfVar3;
  struct {
    RwV3d local_64;
    RwV3d local_58;
    undefined1 pad_4c[8];
    float local_44;
    RwMatrix local_40;
  } frame;
  
  frame.local_58.x = 0.0;
  frame.local_58.y = -1.0;
  frame.local_58.z = 0.0;
  *param_3 = 0.0;
  course_editor_read_chunk();
  iVar1 = gamesave_ptr_to_line_exceptions();
  if (iVar1 != 0) {
    pfVar3 = (float *)(iVar1 + param_1 * 0xc);
    if (pfVar3[2] != -1.0) {
      *param_3 = pfVar3[2];
    }
    if (*pfVar3 != -1.0) {
      RwMatrixRotate(&frame.local_40,(RwV3d *)&g_car_62ac68,k_f_90 - *pfVar3,rwCOMBINEREPLACE);
      RwV3dTransformVectors(param_2,&frame.local_58,1,&frame.local_40);
      RwMatrixRotate(&frame.local_40,(RwV3d *)&g_car_62ac74,pfVar3[1] + k_f_180,rwCOMBINEREPLACE);
      RwV3dTransformVectors(param_2,param_2,1,&frame.local_40);
      return;
    }
  }
  pRVar2 = (RwReal *)course_get_record_base(param_1);
  param_2->x = *pRVar2;
  param_2->y = pRVar2[1];
  param_2->z = pRVar2[2];
  pfVar3 = (float *)course_get_record_point(param_1,0);
  frame.local_64.x = *pfVar3;
  frame.local_64.y = pfVar3[1];
  frame.local_64.z = pfVar3[2];
  pfVar3 = (float *)course_get_record_point(param_1,3);
  frame.local_44 = pfVar3[2];
  frame.local_64.x = frame.local_64.x - *pfVar3;
  frame.local_64.y = frame.local_64.y - pfVar3[1];
  frame.local_64.z = frame.local_64.z - frame.local_44;
  RwMatrixRotate(&frame.local_40,&frame.local_64,-25.0,rwCOMBINEREPLACE);
  RwV3dTransformVectors(param_2,param_2,1,&frame.local_40);
  return;
}


