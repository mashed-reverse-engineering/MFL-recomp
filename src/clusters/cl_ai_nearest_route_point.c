// cluster-entries: ai_nearest_route_point
/* Installed cluster ai_nearest_route_point (tools/cluster_install.py from output/cluster_pilot_md/ai_nearest_route_point.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00418db0*/ extern int ai_bot_set_field_24();
/*0044c7e0*/ extern int camera_path_sample();
/*00475730*/ extern int get_viewport_camera();
/*004b2b48*/ extern int _ftol();
/*004c56e0*/ extern void FUN_004c56e0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd628*/ extern float k_f_0_25;
/*005dd634*/ extern float k_f_2;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005df300*/ extern float k_f_0_333333;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
/*007f0284*/ extern struct AiData g_ai_data;
/* ai_nearest_route_point */

/* "nearest point of a route (points at +0x0, count at +0x200) to a position, with a per player
   hint" */


/* a float stored and read back: the original compares the rounded walker position, not the x87 register value */
static float float_barrier(float v) { return v; }

/* round half away from zero, as the original does with fcom / fadd +-0.5 / _ftol */
static int ai_round_away(double v) { return (int)(v > 0.0 ? v + k_f_0_5 : v - k_f_0_5); }

/* is the tile under the walker (x, y) in the route grid a blocked one (tile type 0 or 3)? */
static int ai_tile_blocked(float x, float y)
{
  int gy = ai_round_away(y * k_f_4) - 0x10;
  int gx = ai_round_away(x * k_f_4) - 0x10;
  short s = g_ai_data.grid[((gy + 0x200) >> 3) * 0x80 + ((gx + 0x200) >> 3)];
  if (0 < s && s < 0x200) {
    unsigned char c = ((unsigned char *)&g_ai_data)[0x8000 + (gy & 7) + ((gx & 7) + s * 8) * 8];
    return c == 0 || c == 3;
  }
  return 0;
}

void ai_nearest_route_point(int route,float *pos,float *out_target,int bot_index,int use_bot_hint,int debug_draw)
{
  char unused_char;
  short unused_short;
  bool unused_bool;
  int hint_diff;
  undefined4 camera;
  int i;
  float *tbl_ptr;
  int cand_index;
  int cand_count;
  float tmp_f;
  int seg_idx;
  int done, blocked_any;
  float lo_t, hi_t;   /* bisection bounds of the sample parameter */
  struct {
    float diff_x;
    float diff_z;
    undefined1 dbg_col_r;
    undefined1 dbg_col_g;
    undefined1 dbg_col_b;
    undefined1 dbg_col_a;
    float best_dist_sq;
    float sample_dist_sq;
    float walk_x;
    float walk_z;
    float dbg_pos_z;
    float dbg_pos_x;
    RwV2d v2_tmp;
    float sample_x;
    float sample_z;
    float hi_dist_sq;
    float best_x;
    float best_z;
    float view_origin_x;
    float view_origin_z;
    RwV2d dir_vec;
    float hi_x;
    float hi_z;
    float cand_pts [64];
    float align_score [16];
  } frame;
  
  frame.dbg_col_r = 0xff;
  frame.dbg_col_g = 0xff;
  frame.dbg_col_b = 0;
  frame.dbg_col_a = 0xff;
  /* debug overlay (editor view): initial colour yellow (255,255,0), view origin = editor view origin +-4 */
  if (debug_draw == 1) {
    frame.view_origin_x = g_ai_editor_view_origin_x + k_f_4;
    frame.view_origin_z = g_ai_editor_view_origin_z - k_f_4;
  }
  cand_count = *(int *)(route + 0x200);
  seg_idx = 0;
  i = 0;
  /* 1: nearest route vertex to pos (squared xz distance) */
  frame.best_dist_sq = 100000.0;
  if (0 < cand_count) {
    do {
      frame.diff_x = *(float *)(route + i * 8) - *pos;
      frame.diff_z = *(float *)(route + 4 + i * 8) - pos[1];
      tmp_f = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
      frame.cand_pts[i] = tmp_f;
      if (tmp_f < frame.best_dist_sq) {
        seg_idx = i;
        frame.best_dist_sq = tmp_f;
      }
      i = i + 1;
    } while (i < cand_count);
  }
  /* 2: per bot hint: do not jump more than one vertex from the bot's last vertex (bot_state[bot].+4) */
  if (use_bot_hint != 0) {
    *(int *)&frame.sample_x = bot_index * 5;
    i = *(int *)(g_ai_data.bot_state + bot_index * 0x14 + 4);
    hint_diff = seg_idx - i;
    if ((hint_diff != 1 - cand_count) && (((hint_diff < -1 || (1 < hint_diff)) && (i < cand_count)))) {
      seg_idx = i;
    }
    *(int *)(g_ai_data.bot_state + bot_index * 0x14 + 4) = seg_idx;
  }
  /* 3: refine on the neighbouring segment: sample points of segment seg_idx and seg_idx-1 near their ends */
  camera_path_sample(route,seg_idx,0x3c23d70a,&frame.sample_x);
  frame.diff_x = frame.sample_x - *pos;
  cand_index = seg_idx + -1;
  frame.diff_z = frame.sample_z - pos[1];
  frame.sample_dist_sq = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
  if (cand_index < 0) {
    cand_index = *(int *)(route + 0x200) + -1;
  }
  camera_path_sample(route,cand_index,0x3f7d70a4,&frame.v2_tmp);
  frame.diff_x = frame.v2_tmp.x - *pos;
  frame.diff_z = frame.v2_tmp.y - pos[1];
  if (frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z < frame.sample_dist_sq) {
    frame.sample_x = frame.v2_tmp.x;
    frame.sample_z = frame.v2_tmp.y;
    seg_idx = cand_index;
  }
  /* 4: 16 step ternary-style search for the nearest parameter t on the segment */
  hi_t = 1.0;
  camera_path_sample(route,seg_idx,0x3f800000,&frame.hi_x);
  frame.diff_x = frame.hi_x - *pos;
  frame.diff_z = frame.hi_z - pos[1];
  frame.hi_dist_sq = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
  lo_t = 0.0;
  camera_path_sample(route,seg_idx,0,&frame.walk_x);
  frame.diff_x = frame.walk_x - *pos;
  cand_index = 0x10;
  frame.diff_z = frame.walk_z - pos[1];
  frame.best_dist_sq = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
  do {
    if (!(frame.hi_dist_sq > frame.best_dist_sq)) {
      lo_t = (lo_t + lo_t + hi_t) * k_f_0_333333;
      camera_path_sample(route,seg_idx,*(int *)&lo_t,&frame.walk_x);
      frame.diff_x = frame.walk_x - *pos;
      frame.diff_z = frame.walk_z - pos[1];
      frame.best_dist_sq = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
    }
    else {
      hi_t = (hi_t + hi_t + lo_t) * k_f_0_333333;
      camera_path_sample(route,seg_idx,*(int *)&hi_t,&frame.hi_x);
      frame.diff_x = frame.hi_x - *pos;
      frame.diff_z = frame.hi_z - pos[1];
      frame.hi_dist_sq = frame.diff_x * frame.diff_x + frame.diff_z * frame.diff_z;
    }
    cand_index = cand_index + -1;
  } while (cand_index != 0);
  if (!(frame.hi_dist_sq > frame.best_dist_sq)) {
    frame.best_z = frame.hi_z;
    frame.best_dist_sq = hi_t;
    tmp_f = frame.hi_x;
  }
  else {
    frame.best_z = frame.walk_z;
    frame.best_dist_sq = lo_t;
    tmp_f = frame.walk_x;
  }
  frame.best_x = tmp_f;
  if (debug_draw != 0) {
    frame.dbg_pos_z = (pos[1] - frame.view_origin_z) * k_f_0_125;
    frame.dbg_pos_x = (frame.view_origin_x - *pos) * k_f_0_125;
    frame.diff_x = (*(float *)(route + 4 + seg_idx * 8) - frame.view_origin_z) * k_f_0_125;
    frame.diff_z = (frame.view_origin_x - *(float *)(route + seg_idx * 8)) * k_f_0_125;
    if ((((((k_f_n1 < frame.diff_x) && (k_f_n1 < frame.dbg_pos_z)) &&
          ((frame.diff_x < k_f_2 && ((frame.dbg_pos_z < k_f_2 && (k_f_n1 < frame.diff_z)))))) &&
         (k_f_n1 < frame.dbg_pos_x)) &&
        ((((frame.diff_z < k_f_2 && (frame.dbg_pos_x < k_f_2)) && (k_f_n1 < frame.diff_z)) &&
         ((k_f_n1 < frame.dbg_pos_x && (frame.diff_z < k_f_2)))))) && (frame.dbg_pos_x < k_f_2)) {
      camera = get_viewport_camera(0);
      FUN_004c56e0(camera,(undefined4)&frame.diff_x,(undefined4)&frame.dbg_pos_z,(undefined4)&frame.dbg_col_r);
    }
    frame.dbg_col_r = 0;
    frame.diff_x = (frame.best_z - frame.view_origin_z) * k_f_0_125;
    frame.diff_z = (frame.view_origin_x - frame.best_x) * k_f_0_125;
    if ((((k_f_n1 < frame.diff_x) && (k_f_n1 < frame.dbg_pos_z)) &&
        (((frame.diff_x < k_f_2 &&
          (((frame.dbg_pos_z < k_f_2 && (k_f_n1 < frame.diff_z)) && (k_f_n1 < frame.dbg_pos_x)))) &&
         (((frame.diff_z < k_f_2 && (frame.dbg_pos_x < k_f_2)) && (k_f_n1 < frame.diff_z)))))) &&
       (((k_f_n1 < frame.dbg_pos_x && (frame.diff_z < k_f_2)) && (frame.dbg_pos_x < k_f_2)))) {
      camera = get_viewport_camera(0);
      FUN_004c56e0(camera,(undefined4)&frame.diff_x,(undefined4)&frame.dbg_pos_z,(undefined4)&frame.dbg_col_r);
    }
  }
  /* 5: walk 16 points ahead along the route (0.05 steps), score each by alignment with the walk direction */
  frame.sample_dist_sq = frame.best_dist_sq;
  frame.walk_z = frame.best_z;
  cand_index = 0;
  tbl_ptr = frame.cand_pts;
  frame.walk_x = tmp_f;
  do {
    frame.sample_dist_sq = frame.sample_dist_sq + k_f_0_05;
    if (float_1_0 < frame.sample_dist_sq) {
      frame.sample_dist_sq = frame.sample_dist_sq - float_1_0;
      seg_idx = seg_idx + 1;
      if (seg_idx == *(int *)(route + 0x200)) {
        seg_idx = 0;
      }
    }
    camera_path_sample(route,seg_idx,*(int *)&frame.sample_dist_sq,&frame.sample_x);
    frame.dir_vec.x = frame.sample_x - frame.walk_x;
    frame.dir_vec.y = frame.sample_z - frame.walk_z;
    RwV2dNormalize(&frame.dir_vec,&frame.dir_vec);
    frame.v2_tmp.x = frame.walk_x - *pos;
    frame.v2_tmp.y = frame.walk_z - pos[1];
    RwV2dNormalize(&frame.v2_tmp,&frame.v2_tmp);
    tmp_f = frame.dir_vec.x * frame.v2_tmp.x + frame.dir_vec.y * frame.v2_tmp.y;
    if (tmp_f < k_f_0) {
      tmp_f = -frame.sample_dist_sq;
    }
    if (float_1_0 < tmp_f) {
      tmp_f = float_1_0;
    }
    frame.align_score[cand_index] = tmp_f;
    frame.walk_z = frame.sample_z;
    *tbl_ptr = frame.sample_x;
    frame.walk_x = frame.sample_x;
    tbl_ptr[1] = frame.sample_z;
    if (debug_draw != 0) {
      frame.dbg_col_b = 0xff;
      frame.dbg_col_g = 0;
      frame.diff_x = (frame.sample_z - frame.view_origin_z) * k_f_0_125;
      frame.diff_z = (frame.view_origin_x - frame.sample_x) * k_f_0_125;
    }
    cand_index = cand_index + 1;
    tbl_ptr = tbl_ptr + 2;
  } while (cand_index < 0x10);
  cand_index = 0;
  /* pick the candidate with the best (largest) alignment score */
  tbl_ptr = frame.align_score + 1;
  seg_idx = 2;
  tmp_f = k_f_0;
  do {
    if (tmp_f < tbl_ptr[-1]) {
      cand_index = seg_idx + -2;
      tmp_f = tbl_ptr[-1];
    }
    if (tmp_f < *tbl_ptr) {
      cand_index = seg_idx + -1;
      tmp_f = *tbl_ptr;
    }
    if (tmp_f < tbl_ptr[1]) {
      tmp_f = tbl_ptr[1];
      cand_index = seg_idx;
    }
    if (tmp_f < tbl_ptr[2]) {
      cand_index = seg_idx + 1;
      tmp_f = tbl_ptr[2];
    }
    if (tmp_f < tbl_ptr[3]) {
      cand_index = seg_idx + 2;
      tmp_f = tbl_ptr[3];
    }
    if (tmp_f < tbl_ptr[4]) {
      cand_index = seg_idx + 3;
      tmp_f = tbl_ptr[4];
    }
    if (tmp_f < tbl_ptr[5]) {
      cand_index = seg_idx + 4;
      tmp_f = tbl_ptr[5];
    }
    if (tmp_f < tbl_ptr[6]) {
      cand_index = seg_idx + 5;
      tmp_f = tbl_ptr[6];
    }
    i = seg_idx + 6;
    tbl_ptr = tbl_ptr + 8;
    seg_idx = seg_idx + 8;
  } while (i < 0x10);
  tmp_f = frame.cand_pts[cand_index * 2 + 1];
  *out_target = frame.cand_pts[cand_index * 2];
  out_target[1] = tmp_f;
  if (debug_draw != 0) {
    frame.dbg_col_b = 0;
    frame.dbg_col_r = 0xff;
    frame.dbg_col_g = 0;
    frame.diff_x = (frame.cand_pts[cand_index * 2 + 1] - frame.view_origin_z) * k_f_0_125;
    frame.diff_z = (frame.view_origin_x - frame.cand_pts[cand_index * 2]) * k_f_0_125;
    if (((((k_f_n1 < frame.diff_x) && (k_f_n1 < frame.dbg_pos_z)) && (frame.diff_x < k_f_2)) &&
        ((frame.dbg_pos_z < k_f_2 && (k_f_n1 < frame.diff_z)))) &&
       ((k_f_n1 < frame.dbg_pos_x && ((frame.diff_z < k_f_2 && (frame.dbg_pos_x < k_f_2)))))) {
      camera = get_viewport_camera(0);
      FUN_004c56e0(camera,(undefined4)&frame.diff_x,(undefined4)&frame.dbg_pos_z,(undefined4)&frame.dbg_col_r);
    }
  }
  /* 6: reject the candidate when the straight line pos -> target crosses a blocked tile; step back to an earlier candidate */
  done = 0;
  blocked_any = 0;
  for (;;) {
    int walker_blocked;
    double t;

    frame.v2_tmp.x = *out_target - *pos;
    frame.sample_dist_sq = 0.0;
    frame.v2_tmp.y = out_target[1] - pos[1];
    lo_t = RwV2dLength(&frame.v2_tmp);
    frame.dir_vec.x = *pos;
    frame.dir_vec.y = pos[1];
    walker_blocked = 0;
    /* pass 1: walk the straight segment current -> target in steps of 0.25 */
    if (k_f_0 < lo_t) {
      t = frame.sample_dist_sq;
      do {
        if (walker_blocked) break;
        frame.walk_x = (float)(frame.v2_tmp.x * (float_1_0 / lo_t) * t + *pos);
        frame.walk_z = (float)((float_1_0 / lo_t) * frame.v2_tmp.y * t + pos[1]);
        if (float_barrier(frame.walk_x) != frame.dir_vec.x && float_barrier(frame.walk_z) != frame.dir_vec.y) {
          if (ai_tile_blocked(frame.walk_x, frame.walk_z)) {
            walker_blocked = 1;
            blocked_any = 1;
          }
        }
        t = t + k_f_0_25;
      } while (t < lo_t);
    }
    if (!walker_blocked) {
      /* pass 2: the same walk along the segment shifted sideways by a quarter step */
      frame.v2_tmp.x = *out_target - *pos;
      frame.sample_dist_sq = 0.0;
      frame.v2_tmp.y = out_target[1] - pos[1];
      lo_t = RwV2dLength(&frame.v2_tmp);
      walker_blocked = 0;
      frame.hi_z = (float)((float_1_0 / lo_t) * frame.v2_tmp.y);
      frame.best_x = (float)(*pos - ((float_1_0 / lo_t) * frame.v2_tmp.y) * k_f_0_25);
      frame.hi_x = (float)((float_1_0 / lo_t) * frame.v2_tmp.x);
      frame.best_z = (float)(pos[1] - ((float_1_0 / lo_t) * frame.v2_tmp.x) * k_f_0_25);
      frame.dir_vec.x = frame.best_x;
      frame.dir_vec.y = frame.best_z;
      if (k_f_0 < lo_t) {
        t = frame.sample_dist_sq;
        do {
          if (walker_blocked) break;
          frame.walk_x = (float)(frame.hi_x * t + frame.best_x);
          frame.walk_z = (float)(frame.hi_z * t + frame.best_z);
          if (float_barrier(frame.walk_x) != frame.best_x && float_barrier(frame.walk_z) != frame.best_z) {
            if (ai_tile_blocked(frame.walk_x, frame.walk_z)) {
              walker_blocked = 1;
              blocked_any = 1;
            }
          }
          t = t + k_f_0_25;
        } while (t < lo_t);
      }
      else {
        done = 1;
      }
    }
    if (done == 0) {
      if (walker_blocked != 1) {
        done = 1;
      }
      else {
        if (cand_index == 0 || cand_index == 1) {
          cand_index = 0;
          done = 1;
        }
        else {
          cand_index = cand_index - 2;
        }
        *(int *)&out_target[0] = *(int *)&frame.cand_pts[cand_index * 2];
        *(int *)&out_target[1] = *(int *)&frame.cand_pts[cand_index * 2 + 1];
      }
    }
    ai_bot_set_field_24(bot_index, cand_index == 0);
    if (done != 0) {
      if (blocked_any != 0) {
        if (cand_index != 0 && cand_index != 1) cand_index = cand_index - 2;
        else cand_index = 0;
        *(int *)&out_target[0] = *(int *)&frame.cand_pts[cand_index * 2];
        *(int *)&out_target[1] = *(int *)&frame.cand_pts[cand_index * 2 + 1];
      }
      if (debug_draw != 0) {
        frame.dbg_col_b = 0;
        frame.dbg_col_r = 0xff;
        frame.dbg_col_g = 0;
        frame.diff_x = (frame.cand_pts[cand_index * 2 + 1] - frame.view_origin_z) * k_f_0_125;
        frame.diff_z = (frame.view_origin_x - frame.cand_pts[cand_index * 2]) * k_f_0_125;
        if (((((k_f_n1 < frame.diff_x) && (k_f_n1 < frame.dbg_pos_z)) && (frame.diff_x < k_f_2)) &&
            ((frame.dbg_pos_z < k_f_2 && (k_f_n1 < frame.diff_z)))) &&
           ((k_f_n1 < frame.dbg_pos_x && ((frame.diff_z < k_f_2 && (frame.dbg_pos_x < k_f_2)))))) {
          camera = get_viewport_camera(0);
          FUN_004c56e0(camera,(undefined4)&frame.diff_x,(undefined4)&frame.dbg_pos_z,(undefined4)&frame.dbg_col_r);
        }
      }
      return;
    }
  }
}

