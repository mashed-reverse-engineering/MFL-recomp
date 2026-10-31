#include "mfl_tier2.h"

/* Adds the point p to a list of float4 points (count, pts) that describes a polyline along the direction d: p is rejected (0) when its distance
   from the line through the last point along d is below 0.001 (squared 1e-6), reported (1) when it lies that close to the first point
   (list of two or more points), and otherwise appended, moved along d so that its dot product with d equals `level`.  Returns 0 / 1. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_aggregate_00588b40(unsigned int *count, float *pts, float *p, float *d, float level)
{
  unsigned int n = *count;
  float *last, vz, t, rx;
  double vx, vy, ry, rz, dot, tot, u;

  if (n > 0) {
    last = pts + n * 4 - 4;
    vx = p[0] - last[0];
    vy = p[1] - last[1];
    vz = p[2] - last[2];
    dot = d[1] * vy;
    dot = dot + vx * d[0];
    dot = dot + d[2] * vz;
    t = -dot;
    rx = t * d[0] + vx;
    ry = t * d[1] + vy;
    rz = t * d[2] + vz;
    tot = ry * ry + rx * rx;
    tot = tot + rz * rz;
    if (tot < 1.0e-6f) return 0;
  }
  if (n > 1) {
    vx = p[0] - pts[0];
    vy = p[1] - pts[1];
    vz = p[2] - pts[2];
    dot = d[1] * vy;
    dot = dot + vx * d[0];
    dot = dot + d[2] * vz;
    t = -dot;
    rx = t * d[0] + vx;
    ry = t * d[1] + vy;
    rz = t * d[2] + vz;
    tot = ry * ry + rx * rx;
    tot = tot + rz * rz;
    if (tot < 1.0e-6f) return 1;
  }
  dot = p[1] * d[1];
  dot = dot + p[0] * d[0];
  dot = dot + p[2] * d[2];
  u = level - dot;
  pts[n * 4] = u * d[0] + p[0];
  pts[*count * 4 + 1] = u * d[1] + p[1];
  pts[*count * 4 + 2] = u * d[2] + p[2];
  ++*count;
  return 0;
}
