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

/* (inferred) course +0xc9f4..0xca0c: two colour / fog near / fog far key frames (A at c9f4, B at ca00) and a roll angle (ca0c); the blend factor is (dot(camera forward flattened, forward rotated by roll) - 1) * -0.5 */
void course_camera_roll_update(char *course,RwCamera *camera)

{
  float unused_f;
  void *camera_frame;
  float fog_near;
  undefined1 unused_a;
  undefined1 unused_b;
  undefined1 unused_c;
  undefined1 unused_d;
  undefined1 unused_e;
  undefined1 unused_g;
  uchar rgb[8];
  RwV3d view_dir_flat;
  RwV3d forward_vec;
  RwV3d up_axis;
  RwMatrix roll_matrix;
  
  camera_frame = RwCamera_object_object_parent(camera);
  forward_vec.x = 0.0;
  forward_vec.y = 0.0;
  forward_vec.z = 1.0;
  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  if (((*(char *)(course + 0xca00) != '\0') || (*(char *)(course + 0xca01) != '\0')) ||
     (*(char *)(course + 0xca02) != '\0')) {
    g_course_update_6095c0 = *(undefined4 *)(course + 0xc9f4);
    g_course_update_6095c4 = *(undefined4 *)(course + 0xca00);
    roll_matrix.flags = roll_matrix.flags | 0x20003;
    roll_matrix.at.z = 1.0;
    roll_matrix.up.y = 1.0;
    roll_matrix.right.x = 1.0;
    roll_matrix.up.x = 0.0;
    roll_matrix.right.z = 0.0;
    roll_matrix.right.y = 0.0;
    roll_matrix.at.y = 0.0;
    roll_matrix.at.x = 0.0;
    roll_matrix.up.z = 0.0;
    roll_matrix.pos.z = 0.0;
    roll_matrix.pos.y = 0.0;
    roll_matrix.pos.x = 0.0;
    RwMatrixRotate(&roll_matrix,&up_axis,*(RwReal *)(course + 0xca0c),rwCOMBINEREPLACE);
    RwV3dTransformVector(&forward_vec,&forward_vec,&roll_matrix);
    view_dir_flat.x = *(float *)((char *)camera_frame + 0x30);
    view_dir_flat.y = 0.0;
    view_dir_flat.z = *(float *)((char *)camera_frame + 0x38);
    RwV3dNormalize(&view_dir_flat,&view_dir_flat);
    g_course_lap_data_split_sectors_14_ = (view_dir_flat.z * forward_vec.z + view_dir_flat.x * forward_vec.x + view_dir_flat.y * forward_vec.y - float_1_0) * k_f_n0_5;
    rgb[0] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[0] - ((uchar *)&g_course_update_6095c0)[0]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[0]);
    rgb[1] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[1] - ((uchar *)&g_course_update_6095c0)[1]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[1]);
    rgb[2] = (uchar)(int)((float)(int)(((uchar *)&g_course_update_6095c4)[2] - ((uchar *)&g_course_update_6095c0)[2]) * g_course_lap_data_split_sectors_14_ + (float)(int)((uchar *)&g_course_update_6095c0)[2]);
    game_set_option_flags(*(int *)&rgb[0],*(int *)&rgb[1],*(int *)&rgb[2]);
    fog_near = (*(float *)(course + 0xca04) - *(float *)(course + 0xc9f8)) * g_course_lap_data_split_sectors_14_ + *(float *)(course + 0xc9f8);
    RwCameraSetFarClipPlane
              (camera,(*(float *)(course + 0xca08) - *(float *)(course + 0xc9fc)) *
                      g_course_lap_data_split_sectors_14_ + *(float *)(course + 0xc9fc));
    RwCamera_fogPlane(camera) = fog_near;
  }
  return;
}


