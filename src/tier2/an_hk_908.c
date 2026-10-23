#include "mfl_tier2.h"
/*004a1860*/ extern int game_set_option_flags(int p0, int p1, int p2);
/*004b2b48*/ extern int _ftol();
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d56a0*/ extern struct RwV3d * RwV3dTransformVector(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*005dd330*/ extern float float_1_0;
/*005de6a8*/ extern float k_f_n0_5;
/*006095c0*/ extern undefined4 g_course_update_6095c0;
/*006095c4*/ extern undefined4 g_course_update_6095c4;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*0068b8ec*/ extern float g_course_lap_data_split_sectors_14_;
/* course_camera_roll_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes the camera roll factor from the camera facing and calls game_set_option_flags" */

void course_camera_roll_update(char *param_1,RwCamera *camera)

{
  float fVar1;
  void *pvVar2;
  float fVar3;
  undefined1 uVar4;
  undefined1 uVar5;
  undefined1 uVar6;
  undefined1 uStack_69;
  undefined1 uStack_68;
  undefined1 uStack_67;
  uchar rgb[8];
  RwV3d local_64;
  RwV3d local_58;
  RwV3d local_4c;
  RwMatrix local_40;
  
  pvVar2 = RwCamera_object_object_parent(camera);
  local_58.x = 0.0;
  local_58.y = 0.0;
  local_58.z = 1.0;
  local_4c.x = 0.0;
  local_4c.y = 1.0;
  local_4c.z = 0.0;
  if (((*(char *)(param_1 + 0xca00) != '\0') || (*(char *)(param_1 + 0xca01) != '\0')) ||
     (*(char *)(param_1 + 0xca02) != '\0')) {
    g_course_update_6095c0 = *(undefined4 *)(param_1 + 0xc9f4);
    g_course_update_6095c4 = *(undefined4 *)(param_1 + 0xca00);
    local_40.flags = local_40.flags | 0x20003;
    local_40.at.z = 1.0;
    local_40.up.y = 1.0;
    local_40.right.x = 1.0;
    local_40.up.x = 0.0;
    local_40.right.z = 0.0;
    local_40.right.y = 0.0;
    local_40.at.y = 0.0;
    local_40.at.x = 0.0;
    local_40.up.z = 0.0;
    local_40.pos.z = 0.0;
    local_40.pos.y = 0.0;
    local_40.pos.x = 0.0;
    RwMatrixRotate(&local_40,&local_4c,*(RwReal *)(param_1 + 0xca0c),rwCOMBINEREPLACE);
    RwV3dTransformVector(&local_58,&local_58,&local_40);
    local_64.x = *(float *)((char *)pvVar2 + 0x30);
    local_64.y = 0.0;
    local_64.z = *(float *)((char *)pvVar2 + 0x38);
    RwV3dNormalize(&local_64,&local_64);
    g_course_lap_data_split_sectors_14_ = (local_64.z * local_58.z + local_64.x * local_58.x + local_64.y * local_58.y - float_1_0) * k_f_n0_5;
    rgb[0] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[0] - ((uchar *)&g_course_update_6095c0)[0]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[0]);
    rgb[1] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[1] - ((uchar *)&g_course_update_6095c0)[1]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[1]);
    rgb[2] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[2] - ((uchar *)&g_course_update_6095c0)[2]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[2]);
    game_set_option_flags(*(int *)&rgb[0],*(int *)&rgb[1],*(int *)&rgb[2]);
    fVar3 = (*(float *)(param_1 + 0xca04) - *(float *)(param_1 + 0xc9f8)) * g_course_lap_data_split_sectors_14_ + *(float *)(param_1 + 0xc9f8);
    RwCameraSetFarClipPlane
              (camera,(*(float *)(param_1 + 0xca08) - *(float *)(param_1 + 0xc9fc)) *
                      g_course_lap_data_split_sectors_14_ + *(float *)(param_1 + 0xc9fc));
    RwCamera_fogPlane(camera) = fVar3;
  }
  return;
}


