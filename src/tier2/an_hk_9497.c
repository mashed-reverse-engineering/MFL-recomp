#include "mfl_tier2.h"
/*0044a470*/ extern int camera_apply_rotation();
/*00481fd0*/ extern double random_range();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddad4*/ extern float k_f_90;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b4*/ extern float k_f_0_0625;
/*005de4ac*/ extern float k_f_270;
/*005de8c4*/ extern float k_f_0_016666668;
/*005df2f4*/ extern float k_f_0_012048192;
/*005df2f8*/ extern float k_f_0_0042918455;
/*005df2fc*/ extern float k_f_0_012820513;
/*007ef804*/ extern undefined4 g_tick_number;
extern double atan2(double, double);
extern double sin(double);
/* camera_record_set_path: points a camera record at a path leg.
 *
 * The camera starts at (start_x, start_y, start_z) and looks towards (end_x, end_y, end_z): the path start and end are stored in the
 * record, then yaw (0x38) and pitch (0x34) are derived from the direction in degrees.  dist_base is the camera distance; dist_sway
 * scales three slow sine waves of the global tick counter that make the yaw, pitch and distance drift so the shot never looks static.
 * Finally camera_apply_rotation builds the view matrix from the record.  (Use: inferred to be the intro / flyover camera paths.) */

typedef struct CameraRecord { /* the part of the camera record that camera_record_set_path fills */
  char unknown_00[0x34];
  float pitch;        /* 0x34: degrees */
  float yaw;          /* 0x38: degrees */
  float roll;         /* 0x3c */
  RwV3d pos;          /* 0x40: current eye position (starts at the path start) */
  char unknown_4c[0xc];
  float distance;     /* 0x58: base distance plus a slow sway */
  char unknown_5c[0x908];
  RwV3d path_start;   /* 0x964 */
  char unknown_970[0xc];
  RwV3d path_end;     /* 0x97c */
} CameraRecord;

void __cdecl camera_record_set_path(CameraRecord *camera_rec,float end_x,float end_y,float end_z,float start_x,float start_y,
          float start_z,float dist_base,float dist_sway)

{
  float dy;
  float dz;
  double angle_deg;
  double fVar4;
  double fVar5;
  double fVar6;
  double fVar7;
  RwReal horiz_len;
  float dx_or_horiz;
  RwV3d delta;
  float sumf;
  
  dx_or_horiz = end_x - start_x;
  camera_rec->path_start.x = start_x;
  dy = end_y - start_y;
  camera_rec->path_start.y = start_y;
  camera_rec->path_start.z = start_z;
  dz = end_z - start_z;
  camera_rec->path_end.x = end_x;
  camera_rec->path_end.y = end_y;
  camera_rec->path_end.z = end_z;
  camera_rec->pos.y = start_y;
  camera_rec->pos.x = start_x;
  camera_rec->pos.z = start_z;
  delta.x = dx_or_horiz;
  delta.y = dy;
  delta.z = dz;
  RwV3dLength(&delta);
  angle_deg = (double)sin((double)(int)g_tick_number * (double)k_f_0_05);
  camera_rec->distance =
       (float)((angle_deg + (double)float_1_0) * (double)dist_base * (double)dist_sway *
               (double)k_f_0_05 + (double)*(volatile float *)&dist_base);
  if (dz == k_f_0) {
    if (k_f_0 <= (double)dx_or_horiz) {
      angle_deg = (double)k_f_90;
    }
    else {
      angle_deg = (double)k_f_270;
    }
  }
  else {
    angle_deg = (double)atan2((double)dx_or_horiz / (double)dz,(double)1);
    angle_deg = (double)k_f_180 - -(angle_deg * (double)(57.295799255371094));
    if (k_f_0 < dz) {
      angle_deg = angle_deg + (double)k_f_180;
    }
  }
  camera_rec->yaw = (float)angle_deg;
  dx_or_horiz = dx_or_horiz * dx_or_horiz + dz * dz;
  if (dx_or_horiz != k_f_0) {
    horiz_len = _rwSqrt(dx_or_horiz);
    if (horiz_len != k_f_0) {
      angle_deg = (double)atan2((double)dy / (double)horiz_len,(double)1);
      angle_deg = (double)k_f_180 - -(angle_deg * (double)(57.295799255371094));
      if (k_f_0 < horiz_len) {
        angle_deg = angle_deg + (double)k_f_180;
      }
      goto LAB_0044c474;
    }
  }
  if (k_f_0 <= dy) {
    angle_deg = (double)k_f_90;
  }
  else {
    angle_deg = (double)k_f_270;
  }
LAB_0044c474:
  fVar7 = (double)k_f_360;
  camera_rec->pitch = (float)(fVar7 - angle_deg);
  fVar4 = (double)(int)g_tick_number;
  fVar5 = (double)sin((double)k_f_0_05 * fVar4);
  fVar6 = (double)sin(fVar4 * (double)k_f_0_012820513);
  fVar4 = (double)sin(fVar4 * (double)k_f_0_0042918455);
  *(volatile float *)&sumf = fVar6 + fVar5;
  camera_rec->pitch =
       (float)(((double)*(volatile float *)&sumf * (double)k_f_0_5 + fVar4) * (double)k_f_0_5 *
               (double)dist_sway + (fVar7 - angle_deg));
  angle_deg = (double)sin((double)k_f_0_1 * (double)(int)g_tick_number);
  fVar7 = (double)sin((double)(int)g_tick_number * (double)k_f_0_012048192);
  dist_base = (float)(fVar7 + angle_deg * (double)k_f_0_125);
  fVar7 = (double)random_range(0,0x42700000);
  angle_deg = (double)(int)g_tick_number;
  camera_rec->roll = 0;
  angle_deg = (double)sin((fVar7 + angle_deg) * (double)k_f_0_016666668);
  camera_rec->yaw =
       (float)((angle_deg * (double)k_f_0_0625 + (double)dist_base) * (double)dist_sway +
              (double)camera_rec->yaw);
  camera_apply_rotation(camera_rec);
  return;
}


