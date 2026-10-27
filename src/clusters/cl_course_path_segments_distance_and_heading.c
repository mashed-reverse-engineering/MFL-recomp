// cluster-entries: course_path_segments_distance_and_heading
/* Installed cluster course_path_segments_distance_and_heading (tools/cluster_install.py from output/cluster_pilot_kb/course_path_segments_distance_and_heading.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040a8a0*/ extern int course_token_list_entry_start();
/*0040a8d0*/ extern int course_token_list_skip_to_entry();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005ddaec*/ extern float k_f_57_29578;
/*005e64a4*/ extern float k_f_0;
/*005de1f0*/ extern double k_f_0_5de1f0; /* 8 byte constant (1.0) */
/*005dfea0*/ extern double k_f_0_5dfea0; /* 8 byte constant (-1.0) */
/* course_path_segments_distance_and_heading */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per path segment computes the length to the next segment and the heading angle (0..360) from the
   direction" */

void course_path_segments_distance_and_heading(int course)

{
  int next_off;
  double dx_or_cos;
  float dz;
  float dy;
  bool more_segments;
  int seg;
  int next_seg;
  int seg_off;
  float10 heading;
  RwReal seg_length;
  struct {
    int path_i;
    int path_row_offset;
    undefined1 pad_1c[8];
    RwV3d dir;
  } frame;
  
  frame.path_i = 0;
  if (0 < *(int *)(course + 0x16244)) {
    frame.path_row_offset = 0;
    do {
      /* per path: walk the segment ring (0x4c-byte records at course+0xc1c0, token-list order); store the distance to the next segment in course[path_row_offset + seg] and the heading angle (0..360 degrees from the direction vector) in course[0x1800 + path_row_offset + seg] */
      more_segments = true;
      seg = course_token_list_entry_start(frame.path_i);
      do {
        next_seg = course_token_list_skip_to_entry(seg, frame.path_i);
        seg_off = course_token_list_entry_start(frame.path_i);
        if (next_seg == seg_off) {
          more_segments = false;
        }
        seg_off = seg * 0x4c + course;
        next_off = next_seg * 0x4c + course;
        dx_or_cos = *(float *)(seg * 0x4c + 0xc1cc + course) - *(float *)(next_off + 0xc1cc);
        dy = *(float *)(seg_off + 0xc1d0) - *(float *)(next_off + 0xc1d0);
        dz = *(float *)(seg_off + 0xc1d4) - *(float *)(next_off + 0xc1d4);
        seg_length = _rwSqrt(dx_or_cos * dx_or_cos + dy * dy + dz * dz);
        *(RwReal *)(course + (frame.path_row_offset + seg) * 4) = seg_length;
        frame.dir.x = *(float *)(seg_off + 0xc1c0);
        frame.dir.y = *(float *)(seg_off + 0xc1c8);
        frame.dir.z = 0.0;
        RwV3dNormalize(&frame.dir,&frame.dir);
        dx_or_cos = frame.dir.z * k_f_0 + frame.dir.x + frame.dir.y * k_f_0;
        if (dx_or_cos < k_f_0_5dfea0) {
          dx_or_cos = k_f_n1;
        }
        if (k_f_0_5de1f0 < dx_or_cos) {
          dx_or_cos = float_1_0;
        }
        heading = (float10)FUN_004b3284((double)dx_or_cos);
        heading = heading * (float10)k_f_57_29578;
        if (frame.dir.x * k_f_0 - frame.dir.y < k_f_0) {
          heading = -heading;
        }
        if (heading < k_f_0 != (heading == k_f_0)) {
          do {
            heading = heading + (float10)k_f_360;
          } while (heading < k_f_0 != (heading == k_f_0));
        }
        *(float *)(course + (frame.path_row_offset + 0x1800 + seg) * 4) = (float)heading;
        seg = next_seg;
      } while (more_segments);
      frame.path_i = frame.path_i + 1;
      frame.path_row_offset = frame.path_row_offset + 0x200;
    } while (frame.path_i < *(int *)(course + 0x16244));
  }
  return;
}

