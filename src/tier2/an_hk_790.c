#include "mfl_tier2.h"
/*004850c0*/ extern int matrix_to_euler_degrees();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005ddc28*/ extern float k_f_0;
/* layout of *path_cam in camera_path_get_point, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct camera_path_get_point_path_cam_layout {
    char pad_0x0[16];
    float f_0x10;
    float f_0x14;
    float f_0x18;
    char pad_0x1c[4];
    float f_0x20;
    float f_0x24;
    float f_0x28;
    char pad_0x2c[4];
    float f_0x30;
    char pad_0x34[4];
    float f_0x38;
    char pad_0x3c[4];
    undefined4 f_0x40;
    undefined4 f_0x44;
    undefined4 f_0x48;
} camera_path_get_point_path_cam_layout;

/*00651248*/ extern char *g_scene_select_param;

/* 180 / pi (float-rounded): radians to degrees */
#define RAD_TO_DEG (57.295780181884766)

/* camera_path_get_point */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills a position / orientation from the path camera frame (inferred)" */

/*
 * Fills out_cam (a camera pose record) from the path camera pointed to by
 * g_scene_select_param->+4: position copied to out_cam+0x4..+0xc, and three
 * Euler angles in degrees written to out_cam+0x10 (roll), +0x14 (pitch),
 * +0x18 (yaw) (inferred). Each angle is the angle between a path-camera
 * vector and a reference axis, found by normalising a vector and taking
 * acos of a dot product.
 * Returns 0 if g_scene_select_param is unset, otherwise 1.
 * Known issue (inferred, check disassembly): the printf before the Euler
 * call passes yaw, roll, pitch under a "roll, pitch, yaw" format, and the
 * values stored to out_cam are the pre-matrix_to_euler_degrees ones.
 */
undefined4 camera_path_get_point(char *out_cam)

{
  camera_path_get_point_path_cam_layout *path_cam;
  float *cam_up;
  double angle;
  struct {
    RwV3d dir;
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
  } frame;
  
  if (g_scene_select_param == 0) {
    return 0;
  }
  /* copy the camera position (path_cam +0x40..+0x48) into out_cam +0x4..+0xc */
  path_cam = (camera_path_get_point_path_cam_layout *)(*(char **)(g_scene_select_param + 4));
  *(undefined4 *)(out_cam + 4) = path_cam->f_0x40;
  cam_up = &path_cam->f_0x10;
  *(undefined4 *)(out_cam + 8) = path_cam->f_0x44;
  *(undefined4 *)(out_cam + 0xc) = path_cam->f_0x48;
  /* roll (inferred): angle of the path vector at +0x20 against the axis
     from dir, normalised with x forced to 0; negated when dir.y >= k_f_0 */
  frame.dir.y = path_cam->f_0x24;
  frame.dir.z = path_cam->f_0x28;
  frame.dir.x = 0.0;
  RwV3dNormalize(&frame.dir,&frame.dir);
  angle = (double)FUN_004b3284((double)(frame.dir.z * path_cam->f_0x28 +
                                        frame.dir.y * path_cam->f_0x24 +
                                        path_cam->f_0x20 * frame.dir.x));
  angle = angle * (double)RAD_TO_DEG;
  if (!(k_f_0 > frame.dir.y)) {
    angle = -angle;
  }
  /* pitch (inferred): same as roll using the vectors at +0x30 / +0x38, y forced to 0;
     negated when dir.z <= k_f_0 */
  frame.dir.x = path_cam->f_0x30;
  frame.roll_deg = (float)angle;
  frame.dir.z = path_cam->f_0x38;
  frame.dir.y = 0.0;
  RwV3dNormalize(&frame.dir,&frame.dir);
  angle = (double)FUN_004b3284((double)(frame.dir.x * k_f_0 + frame.dir.y * k_f_0 + frame.dir.z));
  angle = angle * (double)RAD_TO_DEG;
  if (!(frame.dir.z > k_f_0)) {
    angle = -angle;
  }
  /* yaw (inferred): angle between the vector at +0x10 (cam_up) and (x, y, 0);
     negated when dir.x <= k_f_0 */
  frame.dir.x = *cam_up;
  frame.pitch_deg = (float)angle;
  frame.dir.y = path_cam->f_0x14;
  frame.dir.z = 0.0;
  RwV3dNormalize(&frame.dir,&frame.dir);
  angle = (double)FUN_004b3284((double)(path_cam->f_0x18 * frame.dir.z +
                                        frame.dir.y * path_cam->f_0x14 + frame.dir.x * *cam_up
                                        ));
  angle = angle * (double)RAD_TO_DEG;
  if (!(frame.dir.x > k_f_0)) {
    angle = -angle;
  }
  frame.yaw_deg = (float)angle;
  FID_conflict__wprintf
            ("roll %f, pitch %f, yaw %f\n",(double)frame.yaw_deg,(double)frame.roll_deg,(double)frame.pitch_deg);
  *(float *)(out_cam + 0x10) = frame.roll_deg;
  *(float *)(out_cam + 0x14) = frame.pitch_deg;
  *(float *)(out_cam + 0x18) = frame.yaw_deg;
  /* convert the frame to Euler angles; the result goes back into the same
     three slots, is only printed, and is not copied to out_cam (inferred) */
  matrix_to_euler_degrees(&frame.roll_deg,cam_up);
  FID_conflict__wprintf
            ("roll %f, pitch %f, yaw %f\n",(double)frame.roll_deg,(double)frame.pitch_deg,(double)frame.yaw_deg);
  return 1;
}


