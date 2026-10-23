// cluster-entries: ai_route_closest_point_search
/* Installed cluster ai_route_closest_point_search (tools/cluster_install.py from output/cluster_pilot_uc3/ai_route_closest_point_search.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern int camera_path_sample(void *route, int index, float t, float *out);
extern float atan2_degrees(float dx, float dy);
extern int get_viewport_camera(int index);
extern int FUN_004c56e0(int camera, float *from, float *to, unsigned char *colour);
extern float g_ai_editor_view_origin_x, g_ai_editor_view_origin_z, float_1_0;
extern float k_f_4, k_f_0_333333, k_f_0_01, k_f_0_1, k_f_0_5, k_f_0_125, k_f_n1, k_f_2, k_f_0, k_f_360, k_f_180;

/* Finds the closest point of a route to a position: the closest node by squared distance, refined to the closest of its two neighbouring pieces and then by
   repeated thirds (16 steps) along the spline (camera_path_sample); then walks `distance / 2` and `distance` ahead (steps of 0.1 / 0.01) to get the turn angles
   there.  *out = the heading difference at the half distance; returns the share of the first turn in both turns (0 when they cancel). */
float ai_route_closest_point_search(char *route, float *pos, float distance, float *out, int draw)
{
  unsigned char colour[4];
  float sq[65];
  float s[2];
  float s2[2];
  float prev[2];
  float p_lo[2];
  float p_hi[2];
  float best_pos[2];
  float mark_x;
  float mark_z;
  float d_best;
  float d_lo;
  float d_hi;
  float lo;
  float hi;
  float t;
  float chosen;
  float acc;
  float half;
  float A[2];
  float B[2];
  float d;
  float angle0;
  float angle1;
  float angle2;
  double v;
  double u;
  double w;
  int count;
  int i;
  int best;
  int index;
  int walk;

  colour[0] = 0xff;
  colour[1] = 0xff;
  colour[2] = 0;
  colour[3] = 0xff;
  if (draw == 1) {
    mark_x = g_ai_editor_view_origin_x + k_f_4;
    mark_z = g_ai_editor_view_origin_z - k_f_4;
  }
  count = *(int *)(route + 0x200);
  best = 0;
  d_best = 100000.0f;
  for (i = 0; i < count; i++) {
    A[0] = *(float *)(route + i * 8) - pos[0];
    A[1] = *(float *)(route + i * 8 + 4) - pos[1];
    d = A[1] * A[1] + A[0] * A[0];
    sq[i] = d;
    if (d < d_best) {
      d_best = d;
      best = i;
    }
  }
  camera_path_sample(route, best, 0.01f, s);
  A[0] = s[0] - pos[0];
  A[1] = s[1] - pos[1];
  d_best = A[1] * A[1] + A[0] * A[0];
  i = best - 1;
  if (i < 0) {
    i = *(int *)(route + 0x200) - 1;
  }
  camera_path_sample(route, i, 0.99f, s2);
  A[0] = s2[0] - pos[0];
  A[1] = s2[1] - pos[1];
  if (A[1] * A[1] + A[0] * A[0] < d_best) {
    s[0] = s2[0];
    s[1] = s2[1];
    best = i;
  }
  hi = 1.0f;
  camera_path_sample(route, best, 1.0f, p_hi);
  A[0] = p_hi[0] - pos[0];
  A[1] = p_hi[1] - pos[1];
  d_hi = A[1] * A[1] + A[0] * A[0];
  lo = 0.0f;
  camera_path_sample(route, best, 0.0f, p_lo);
  A[0] = p_lo[0] - pos[0];
  A[1] = p_lo[1] - pos[1];
  d_lo = A[1] * A[1] + A[0] * A[0];
  walk = 0x10;
  do {
    if (!(d_hi > d_lo)) {
      lo = (lo + lo + hi) * k_f_0_333333;
      camera_path_sample(route, best, lo, p_lo);
      A[0] = p_lo[0] - pos[0];
      A[1] = p_lo[1] - pos[1];
      d_lo = A[1] * A[1] + A[0] * A[0];
    }
    else {
      hi = (hi + hi + lo) * k_f_0_333333;
      camera_path_sample(route, best, hi, p_hi);
      A[0] = p_hi[0] - pos[0];
      A[1] = p_hi[1] - pos[1];
      d_hi = A[1] * A[1] + A[0] * A[0];
    }
    walk--;
  } while (walk != 0);
  if (!(d_hi > d_lo)) {
    t = hi;
    best_pos[0] = p_hi[0];
    best_pos[1] = p_hi[1];
  }
  else {
    t = lo;
    best_pos[0] = p_lo[0];
    best_pos[1] = p_lo[1];
  }
  chosen = t;
  t = t + k_f_0_01;
  index = best;
  if (t > float_1_0) {
    t = t - float_1_0;
    index = best + 1;
    if (index == *(int *)(route + 0x200)) {
      index = 0;
    }
  }
  camera_path_sample(route, index, t, s);
  A[0] = s[0] - best_pos[0];
  A[1] = s[1] - best_pos[1];
  angle0 = atan2_degrees(A[0], A[1]);
  half = distance * k_f_0_5;
  prev[0] = best_pos[0];
  prev[1] = best_pos[1];
  t = chosen;
  acc = 0.0f;
  if (k_f_0 < half) {
    do {
      t = t + k_f_0_1;
      if (t > float_1_0) {
        t = t - float_1_0;
        best++;
        if (best == *(int *)(route + 0x200)) {
          best = 0;
        }
      }
      camera_path_sample(route, best, t, s);
      A[0] = s[0] - prev[0];
      A[1] = s[1] - prev[1];
      prev[0] = s[0];
      prev[1] = s[1];
      acc = A[1] * A[1] + A[0] * A[0] + acc;
      if (draw != 0) {
        colour[2] = 0xff;
        colour[1] = 0;
        colour[0] = 0;
        B[0] = (pos[1] - mark_z) * k_f_0_125;
        B[1] = (mark_x - pos[0]) * k_f_0_125;
        A[0] = (s[1] - mark_z) * k_f_0_125;
        A[1] = (mark_x - s[0]) * k_f_0_125;
        if (A[0] > k_f_n1 && B[0] > k_f_n1 && A[0] < k_f_2 && B[0] < k_f_2 && A[1] > k_f_n1 && B[1] > k_f_n1 && A[1] < k_f_2 && B[1] < k_f_2) {
          FUN_004c56e0(get_viewport_camera(0), A, B, colour);
        }
      }
    } while (acc < half);
  }
  t = t + k_f_0_01;
  if (t > float_1_0) {
    t = t - float_1_0;
    best++;
    if (best == *(int *)(route + 0x200)) {
      best = 0;
    }
  }
  camera_path_sample(route, best, t, s);
  A[0] = s[0] - prev[0];
  A[1] = s[1] - prev[1];
  angle1 = atan2_degrees(A[0], A[1]);
  if (acc < distance) {
    do {
      t = t + k_f_0_1;
      if (t > float_1_0) {
        t = t - float_1_0;
        best++;
        if (best == *(int *)(route + 0x200)) {
          best = 0;
        }
      }
      camera_path_sample(route, best, t, s);
      A[0] = s[0] - prev[0];
      A[1] = s[1] - prev[1];
      prev[0] = s[0];
      prev[1] = s[1];
      acc = A[1] * A[1] + A[0] * A[0] + acc;
      if (draw != 0) {
        colour[2] = 0xff;
        colour[1] = 0;
        colour[0] = 0;
        B[0] = (pos[1] - mark_z) * k_f_0_125;
        B[1] = (mark_x - pos[0]) * k_f_0_125;
        A[0] = (s[1] - mark_z) * k_f_0_125;
        A[1] = (mark_x - s[0]) * k_f_0_125;
        if (A[0] > k_f_n1 && B[0] > k_f_n1 && A[0] < k_f_2 && B[0] < k_f_2 && A[1] > k_f_n1 && B[1] > k_f_n1 && A[1] < k_f_2 && B[1] < k_f_2) {
          FUN_004c56e0(get_viewport_camera(0), A, B, colour);
        }
      }
    } while (acc < distance);
  }
  t = t + k_f_0_01;
  if (t > float_1_0) {
    t = t - float_1_0;
    best++;
    if (best == *(int *)(route + 0x200)) {
      best = 0;
    }
  }
  camera_path_sample(route, best, t, s);
  A[0] = s[0] - prev[0];
  A[1] = s[1] - prev[1];
  angle2 = atan2_degrees(A[0], A[1]);
  v = angle0 - angle2;
  while (v < k_f_0) {
    v = v + k_f_360;
  }
  *out = (float)v;
  u = (double)angle0 - angle1;
  w = (double)angle1 - angle2;
  while (u < k_f_0) {
    u = u + k_f_360;
  }
  while (w < k_f_0) {
    w = w + k_f_360;
  }
  if (u > k_f_180) {
    u = -(k_f_360 - u);
  }
  if (w > k_f_180) {
    w = -(k_f_360 - w);
  }
  if (w + u != k_f_0) {
    return (float)(u / (w + u));
  }
  return k_f_0;
}

