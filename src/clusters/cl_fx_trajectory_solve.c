// cluster-entries: fx_trajectory_solve
/* Installed cluster fx_trajectory_solve (tools/cluster_install.py from output/cluster_pilot_md/fx_trajectory_solve.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409040*/ extern int line_exceptions_find_index_a(int record, int list);
/*00409130*/ extern int line_exceptions_find_value(int record, int list);
/*004091d0*/ extern int course_token_list_count_entries(int list);
/*00409210*/ extern int line_exceptions_resolve_index(int list, int record, int count);
/*0042b1c0*/ extern float * course_get_record_pos(int record);
/*0042b1d0*/ extern float * course_get_record_base(int record);
/*0042b210*/ extern float * course_get_record_point(int record, int corner);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd62c*/ extern float k_f_100;
/*005ddb9c*/ extern float k_f_n10;
/*005e64a4*/ extern float k_f_0;
/* fx_trajectory_solve */

/* "inferred: ballistic / aim solution using course_get_record_pos / 1d0 (error string i is too big)" */

/* Result of the track position solve (read by the AI / player route code). */
typedef struct TrajectoryResult {
  int record;         /* track record the point is in */
  float position;     /* record + fraction inside it (wraps at the list length) */
  float percent;      /* position along the list, 0 .. 100 */
  int edge;           /* 1 = inside, 2 / 3 = outside the end / start edge, 4 / 5 = below the lower / upper corner plane */
  int unused_10;
  float margin;       /* signed distance to the violated edge (0 when inside) */
  float edge_dist[4]; /* distances to the four edges of the record */
} TrajectoryResult;

/* a float stored and read back: the original keeps these differences in stack slots (rounded) */
static float float_barrier(float v) { return v; }

/* signed distance of pos to the plane of a record (record position, base vector) */
static float plane_dist(float *pos, float *record_pos, float *base)
{
  RwV3d d;
  d.x = pos[0] - record_pos[0];
  d.y = pos[1] - record_pos[1];
  d.z = pos[2] - record_pos[2];
  return base[2] * d.z + base[1] * d.y + d.x * base[0];
}

void fx_trajectory_solve(int unused_player, int seg, int list, float *pos, TrajectoryResult *out, float margin)
{
  RwV3d d;
  RwV3d dir;
  RwV3d n;
  float *rp;
  float *rb;
  float *pa;
  float *pb;
  double dsum;      /* an edge distance: the original keeps the unrounded x87 value for the margin test */
  float dist;       /* plane distance, later the |.| of the positive side */
  float other;      /* the distance on the other side of the plane crossing */
  float a, b;
  float edge_best;  /* out->margin */
  float frac;
  float c0, c1, c2;
  float ax, ay, az, bx, by, bz;
  float x0, z0, x1, z1;
  int cur;          /* record the crossing is in */
  int next;
  int next2;
  int iter;
  int found;
  int edge;
  int count;
  int prev;
  int resolved;

  cur = seg;
  rp = course_get_record_pos(seg);
  rb = course_get_record_base(seg);
  dist = plane_dist(pos, rp, rb);
  if (!(dist <= k_f_0)) {
    /* in front of the record plane: walk forward until the plane is crossed */
    found = 0;
    iter = 0;
    prev = line_exceptions_find_index_a(cur, list);
    if (prev != -10) {
      cur = prev;
      goto walk_forward_check;
    }
    for (;;) {
      next = line_exceptions_find_value(cur, list);
      if (next > 0x1ff) {
        FID_conflict__wprintf("i is too big!!\n");
      }
      rp = course_get_record_pos(next);
      rb = course_get_record_base(next);
      other = plane_dist(pos, rp, rb);
      if (!(other <= k_f_0)) {   /* NaN counts as positive, like the original's compare */
        dist = -other;
      }
      else {
        found = 1;
      }
      iter = iter + 1;
      if (10 < iter) {
        found = 1;
      }
walk_forward_check:
      if (found) {
        break;
      }
      cur = line_exceptions_find_value(cur, list);
    }
  }
  else {
    if (seg == 0 && list == 0) {
      prev = line_exceptions_find_index_a(0, 0);
      edge_best = k_f_0;
      if (prev == -10) {
        frac = (float)seg;
        out->edge_dist[2] = 0.0f;
        out->edge_dist[3] = 0.0f;
        out->edge_dist[0] = 0.0f;
        out->edge_dist[1] = 0.0f;
        edge = 0;
        goto store_result;
      }
    }
    /* behind the plane: walk backwards until the point is in front of it */
    found = 0;
    iter = 0;
    do {
      other = dist;
      cur = line_exceptions_find_index_a(cur, list);
      rp = course_get_record_pos(cur);
      rb = course_get_record_base(cur);
      dist = plane_dist(pos, rp, rb);
      if (k_f_0 <= dist) {
        found = 1;
      }
      iter = iter + 1;
    } while (iter < 0xb && !found);
  }
  a = dist;
  b = -other;
  if (a < k_f_0) {
    a = -a;
  }
  if (b < k_f_0) {
    b = -b;
  }
  frac = a / (b + a) + (double)cur;
  count = course_token_list_count_entries(list);
  if (count <= frac) {
    frac = frac - course_token_list_count_entries(list);
  }
  edge_best = 0.0f;
  edge = 1;
  next = line_exceptions_find_value(cur, list);
  next2 = line_exceptions_find_value(next, list);
  pa = course_get_record_pos(cur);
  x0 = pa[0];
  z0 = pa[2];
  pb = course_get_record_pos(next);
  x1 = pb[0];
  z1 = pb[2];
  dir.z = x1 - x0;
  dir.y = 0.0f;
  dir.x = -(z1 - z0);
  RwV3dNormalize(&n, &dir);
  d.x = float_barrier(pos[0] - x0);
  d.z = float_barrier(pos[2] - z0);
  out->edge_dist[2] = n.x * d.x + n.z * d.z;
  course_get_record_pos(next);
  pb = course_get_record_pos(next2);
  x0 = pb[0];
  z0 = pb[2];
  dir.z = x1 - x0;
  dir.y = 0.0f;
  dir.x = -(z1 - z0);
  RwV3dNormalize(&n, &dir);
  d.x = float_barrier(pos[0] - x0);
  d.z = float_barrier(pos[2] - z0);
  out->edge_dist[3] = n.x * d.x + n.z * d.z;
  pa = course_get_record_point(cur, 2);
  x0 = pa[0];
  z0 = pa[2];
  pb = course_get_record_point(next, 2);
  dir.z = pb[0] - x0;
  dir.y = 0.0f;
  dir.x = -(pb[2] - z0);
  RwV3dNormalize(&n, &dir);
  d.x = float_barrier(pos[0] - x0);
  d.z = float_barrier(pos[2] - z0);
  dsum = n.x * d.x + n.z * d.z;
  out->edge_dist[0] = (float)dsum;
  if (dsum < margin) {
    edge_best = (float)(dsum - margin);
    edge = 2;
  }
  pa = course_get_record_point(cur, 0);
  x0 = pa[0];
  z0 = pa[2];
  pb = course_get_record_point(next, 0);
  dir.z = pb[0] - x0;
  dir.x = -(pb[2] - z0);
  RwV3dNormalize(&n, &dir);
  d.x = float_barrier(x0 - pos[0]);
  d.z = float_barrier(z0 - pos[2]);
  dsum = n.x * d.x + n.z * d.z;
  out->edge_dist[1] = (float)dsum;
  if (dsum < margin) {
    edge_best = (float)(dsum - margin);
    edge = 3;
  }
  rb = course_get_record_base(cur);
  c0 = rb[0];
  c1 = rb[1];
  c2 = rb[2];
  pa = course_get_record_point(cur, 0);
  ax = pa[0];
  ay = pa[1];
  az = pa[2];
  pb = course_get_record_point(cur, 3);
  bx = float_barrier(pb[0] - ax);
  by = float_barrier(pb[1] - ay);
  bz = pb[2] - az;
  n.x = c1 * bz - c2 * by;
  n.y = c2 * bx - c0 * bz;
  n.z = c0 * by - c1 * bx;
  d.x = float_barrier(pos[0] - ax);
  d.y = float_barrier(pos[1] - ay);
  d.z = float_barrier(pos[2] - az);
  dist = n.y * d.y + n.z * d.z + n.x * d.x;
  if (dist < k_f_n10 && dist < edge_best) {
    edge = 5;
    edge_best = dist;
  }
  pa = course_get_record_point(cur, 2);
  ax = pa[0];
  ay = pa[1];
  az = pa[2];
  pb = course_get_record_point(cur, 1);
  bx = float_barrier(pb[0] - ax);
  by = float_barrier(pb[1] - ay);
  bz = pb[2] - az;
  n.x = c1 * bz - c2 * by;
  n.y = c2 * bx - c0 * bz;
  n.z = c0 * by - c1 * bx;
  d.x = float_barrier(pos[0] - ax);
  d.y = float_barrier(pos[1] - ay);
  d.z = float_barrier(pos[2] - az);
  dist = n.y * d.y + n.z * d.z + n.x * d.x;
  if (dist < k_f_0 && dist < edge_best) {
    edge = 4;
    edge_best = dist;
  }
store_result:
  out->margin = edge_best;
  out->record = cur;
  out->edge = edge;
  out->position = frac;
  count = course_token_list_count_entries(list);
  resolved = line_exceptions_resolve_index(list, cur, count);
  out->percent = (float)(((double)out->position - (double)(int)out->position + resolved) / count * k_f_100);
}

