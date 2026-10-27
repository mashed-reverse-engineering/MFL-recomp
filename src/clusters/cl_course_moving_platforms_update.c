// cluster-entries: course_moving_platforms_update
/* Installed cluster course_moving_platforms_update (tools/cluster_install.py from output/cluster_pilot_kg/course_moving_platforms_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00484c40*/ extern void __cdecl spline_tween_b(RwMatrix *out, int spline, float time);
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004c48b0*/ extern void FUN_004c48b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056b220*/ extern void rwp_world_set_bit_60_64(int p0, int p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, undefined4 p1);
/*0056bc20*/ extern void FUN_0056bc20(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd41c*/ extern float float_5_0;
/*005dd61c*/ extern float k_f_0_001;
/*005dd620*/ extern float float_10_0;
/*005dd7f0*/ extern float k_f_30;
/*005dd9c8*/ extern float k_f_0_16666667;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb34*/ extern float k_f_0_95;
/*005ddc28*/ extern float k_f_0;
/*005de1b0*/ extern float k_f_n0_05;
/*005dfcf4*/ extern float k_f_21;
/*005dfdec*/ extern float k_f_n0_95;
/*007ef7f4*/ extern float g_tick_seconds;
/* course_moving_platforms_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per platform object follows its spline (tween), pushes the physics body by velocity and clamps
   lateral offset" */

void course_moving_platforms_update(char *world_base, char *platform_base)

{
  undefined4 *vec_ptr;
  float body_scale;
  int *body_ptr;
  RwMatrix *body_matrix;
  undefined4 handle_or_z;
  int owner;
  int spline;
  float *platform_ptr;
  RwReal dist;
  double sum;
  double lap;
  double push;
  struct {
    float advance;
    undefined1 pad_c8[4];
    int platform_i;
    RwV3d local_pos;
    int spline_copy;
    undefined4 body_vec_x;
    float body_vec_y;
    undefined1 pad_a8[4];
    float push_x;
    float push_y;
    float push_z;
    float progress;
    undefined4 body_flags;
    RwV3d *body_pos_ptr;
    RwV3d delta;
    RwMatrix pose; /* the pose of the platform on its spline */
    RwMatrix inverse; /* inverse of the physics body matrix */
  } frame;
  
  frame.platform_i = 0;
  if (0 < *(int *)(platform_base + 0x250c)) {
    platform_ptr = (float *)(platform_base + 0x2444);
    do {
      if (platform_ptr[-1] != 0.0) {
        body_matrix = (RwMatrix *)rwp_object_get_matrix(*(int *)&platform_ptr[-6]);
        handle_or_z = rwp_object_get_handle(*(int *)&platform_ptr[-6]);
        frame.advance = g_tick_seconds + g_tick_seconds;
        spline = *(int *)(world_base + 0xc9b0 + *(int *)&platform_ptr[-2] * 4);
        frame.spline_copy = spline;
        if (platform_ptr[-4] == 0.0) {
          FUN_004c48b0(&frame.pose,spline,*(int *)&*platform_ptr);
        }
        else {
          spline_tween_b(&frame.pose,spline,*platform_ptr);
        }
        if (*platform_ptr < k_f_0_001 != (*platform_ptr == k_f_0_001)) {
          rwp_object_set_matrix(*(int *)&platform_ptr[-6],&frame.pose);
        }
        owner = rwp_owner_object_get(handle_or_z);
        rwp_world_set_bit_60_64(*(undefined4 *)(owner + 0x24),owner,1);
        rwp_world_set_bit_5c(*(undefined4 *)(owner + 0x24),owner,1);
        FUN_0056b300(*(undefined4 *)(owner + 0x24),owner);
        frame.body_flags = **(undefined4 **)(owner + 0x10);
        body_ptr = (int *)(*(undefined4 **)(owner + 0x10))[1];
        if (body_ptr != (int *)0x0) {
          body_scale = *(float *)(*(char **)(*(int *)(*body_ptr + 0x10) + 0x20) + 0xc + body_ptr[1] * 0x30);
          frame.body_pos_ptr = &body_matrix->pos;
          frame.delta.x = frame.pose.pos.x - frame.body_pos_ptr->x;
          frame.delta.y = frame.pose.pos.y - (body_matrix->pos).y;
          frame.delta.z = frame.pose.pos.z - (body_matrix->pos).z;
          dist = RwV3dLength(&frame.delta);
          lap = dist * k_f_0_16666667; /* the original keeps this extended precision value in a register for the next two uses */
          frame.progress = (float)lap;
          frame.advance = (float)((float_1_0 - lap) * frame.advance);
          if (float_1_0 < lap) {
            platform_ptr[-1] = 0.0;
          }
          RwMatrixInvert(&frame.inverse,body_matrix);
          RwV3dTransformPoint(&frame.local_pos,&frame.pose.pos,&frame.inverse);
          vec_ptr = (undefined4 *)(*(char **)(*(int *)(*body_ptr + 0x10) + 8) + 0x10 + body_ptr[1] * 0x20);
          frame.body_vec_x = *vec_ptr;
          frame.body_vec_y = *(float *)&vec_ptr[1];
          handle_or_z = vec_ptr[2];
          if ((k_f_0_05 < frame.local_pos.x) || (frame.local_pos.x < k_f_n0_05)) {
            if (frame.local_pos.x <= k_f_0_95) {
              if (frame.local_pos.x < k_f_n0_95) {
                frame.local_pos.x = -0.95;
              }
            }
            else {
              frame.local_pos.x = 0.95;
            }
            frame.body_vec_y = frame.local_pos.x + frame.body_vec_y;
            vec_ptr = (undefined4 *)
                     (*(int *)(*(char **)(*body_ptr + 0x10) + 8) + 0x10 + body_ptr[1] * 0x20);
            *vec_ptr = frame.body_vec_x;
            *(float *)&vec_ptr[1] = frame.body_vec_y;
            vec_ptr[2] = handle_or_z;
            *(float *)&body_ptr[6] = (float_1_0 / (g_tick_seconds * float_5_0));
          }
          push = frame.progress * k_f_30;
          push = push * body_scale;
          frame.push_x = (body_matrix->at).x * push;
          frame.push_y = (body_matrix->at).y * push;
          frame.push_z = (body_matrix->at).z * push;
          if (platform_ptr[-3] == 0.0) {
            frame.push_y = float_10_0;
            if (k_f_0 < frame.delta.y) {
              frame.push_y = k_f_21;
            }
            frame.push_y = body_scale * frame.push_y;
          }
          FUN_0056bc20(body_ptr,&frame.push_x,frame.body_pos_ptr);
          *(float *)&body_ptr[5] = (float_1_0 / (g_tick_seconds * float_10_0));
          FUN_0056ca70(frame.body_flags,0x3c23d70a);
          body_ptr[7] = 0x3c23d70a;
          spline = frame.spline_copy;
        }
        sum = frame.advance + *platform_ptr;
        *platform_ptr = (float)sum;
        if (*(float *)(spline + 0xc) < sum) {
          *platform_ptr = 0.02f;
        }
      }
      frame.platform_i = frame.platform_i + 1;
      platform_ptr = platform_ptr + 7;
    } while (frame.platform_i < *(int *)(platform_base + 0x250c));
  }
  return;
}

