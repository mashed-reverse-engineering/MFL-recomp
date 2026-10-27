// cluster-entries: course_world_ray_hit_light_sample
/* Installed cluster course_world_ray_hit_light_sample (tools/cluster_install.py from output/cluster_pilot_kd/course_world_ray_hit_light_sample.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*00569610*/ extern int RtBaryGetTransform();
/*005dd330*/ extern float float_1_0;
/*005dff00*/ extern float k_f_0_003921569;
/* course_world_ray_hit_light_sample */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "ray test against the course world (world_intersect_line), samples vertex colours via barycentric coords
   for a light factor (0..1)" */

undefined4 course_world_ray_hit_light_sample(char *course, float *line_start, float *out_light, float *out_hit_t)

{
  ushort *tri_indices;
  int vertices;
  float light;
  int vertex_colors_or_hit;
  struct {
    undefined1 bary_transform_dummy [4];
    float hit_x;
    float hit_y;
    float hit_z;
    float bary_m00;
    float bary_m01;
    float bary_m02;
    undefined1 pad_80[4];
    float bary_m10;
    float bary_m11;
    float bary_m12;
    undefined1 pad_70[4];
    float bary_m20;
    float bary_m21;
    float bary_m22;
    undefined1 pad_60[4];
    float bary_m30;
    float bary_m31;
    float bary_m32;
    float bary_m33;  /* 4x4 matrix written by RtBaryGetTransform (64 bytes) */
  } frame;
  struct { /* line intersection result record (filled by world_intersect_line) */
    float seg_dz;
    undefined1 hit_point [12];
    int hit_triangle;     /* triangle index */
    undefined1 pad_30 [0x24];
    int hit_sector;      /* hit sector / object */
    float hit_t;    /* parameter along the segment */
    int hit_tail;      /* last word of the 0x40-byte record, written by the intersect callback */
  } frame2;
  
  /* ray test over the course world; hit parameter t -> out_hit_t */
  vertex_colors_or_hit = world_intersect_line(*(undefined4 *)(course + 0xc090),line_start,frame2.hit_point);
  if (vertex_colors_or_hit != 0) {
    if (out_hit_t != (float *)0x0) {
      *out_hit_t = frame2.hit_t;
    }
    if (*(short *)(frame2.hit_sector + 0x84) != 0) {
      if (out_light != (float *)0x0) {
        vertex_colors_or_hit = *(int *)(frame2.hit_sector + 0x30);
        if (vertex_colors_or_hit != 0) {
          vertices = *(int *)(frame2.hit_sector + 8);
          tri_indices = (ushort *)(*(char **)(frame2.hit_sector + 4) + frame2.hit_triangle * 8);
          frame2.seg_dz = line_start[5] - line_start[2];
          /* hit point on the segment; light = sum over the 3 triangle vertices of (colour byte / 255) * barycentric weight, clamped to 1 */
          frame.hit_x = (line_start[3] - *line_start) * frame2.hit_t + *line_start;
          frame.hit_y = (line_start[4] - line_start[1]) * frame2.hit_t + line_start[1];
          frame.hit_z = frame2.seg_dz * frame2.hit_t + line_start[2];
          RtBaryGetTransform(&frame.bary_m00,frame.bary_transform_dummy,vertices + (uint)*tri_indices * 0xc,
                             vertices + (uint)tri_indices[1] * 0xc,vertices + (uint)tri_indices[2] * 0xc);
          light = (float)(*(uint *)(vertex_colors_or_hit + (uint)*tri_indices * 4) & 0xff) * k_f_0_003921569 *
                  (frame.bary_m00 * frame.hit_x + frame.bary_m10 * frame.hit_y + frame.bary_m20 * frame.hit_z + frame.bary_m30) +
                  (float)(*(uint *)(vertex_colors_or_hit + (uint)tri_indices[1] * 4) & 0xff) * k_f_0_003921569 *
                  (frame.bary_m01 * frame.hit_x + frame.bary_m11 * frame.hit_y + frame.bary_m21 * frame.hit_z + frame.bary_m31) +
                  (float)(*(uint *)(vertex_colors_or_hit + (uint)tri_indices[2] * 4) & 0xff) * k_f_0_003921569 *
                  (frame.bary_m02 * frame.hit_x + frame.bary_m12 * frame.hit_y + frame.bary_m22 * frame.hit_z + frame.bary_m32);
          if (light <= float_1_0) {
            *out_light = light;
            return 1;
          }
          *out_light = float_1_0;
          return 1;
        }
        *out_light = 1.0;
      }
      return 1;
    }
  }
  return 0;
}

