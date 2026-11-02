/* rwp_cylinder_sweep (0x00570620, was gap_570620): moving point against a rounded cylinder */
#include "mfl_tier2.h"
#include <math.h>

/* The segment p -> q against a cylinder along z (half height h, radius r) whose rim is rounded by a torus of tube radius e.  Up to four steps along the
   segment: the part the point is in (flags bit 0 = inside the height slab, bit 1 = inside the radius) selects the surface tests: the rim torus
   (rwp_ray_torus_root_test), the side and outer-radius quadrics (rwp_line_quadric_intersect) and the cap plane (5772c0).  Each hit time is a fraction
   tn / td; the nearest is taken (cross-multiplied compare), the point moved there and the loop repeated until the final contact gives point and normal. */
/*00577750*/ extern int rwp_ray_torus_root_test(float *out, float *center, float *dir, float *origin, float *axis, float r0, float r1);
/*00577550*/ extern int rwp_line_quadric_intersect(float *out, float *a2, float *v, float *a4, float *dir, float r, int a7, int a8);
/*005772c0*/ extern int rwp_grid_005772c0(float *out, float *p, float *d, int kind, int zone, int plane);
/*005771f0*/ extern float rwp_grid_005771f0(float *out, float *in, float *axis, int flag);

typedef struct MflCylCast {
  unsigned char pad00[0x40];
  float half;   /* 0x40 */
  float radius;   /* 0x44 */
  int pad48;
  float edge;   /* 0x4c: radius of the rounded rim */
} MflCylCast;

/* out[0] = the cylinder, [1..3] contact point, [4..6] normal, [7] fraction of the segment travelled */
typedef struct MflCylContact {
  MflCylCast *cyl;
  float pos[3];   /* 0x4 */
  float n[3];   /* 0x10 */
  float t;   /* 0x1c */
} MflCylContact;

#define BITS(x) (*(int *)&(x))

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
int rwp_cylinder_sweep(MflCylCast *cyl, float *p, float *q, MflCylContact *out)
{
  struct {
    volatile float px, py, pz;   /* 0x0 current point (the original keeps it in the frame and reloads it) */
    float tn, td;   /* 0xc hit time as a fraction */
    float nx, ny, nz;   /* 0x14 normal */
    float zone;   /* 0x20 -1 below, 0 inside, +1 above the height slab (the cap plane sign) */
    float er;   /* 0x24 edge + radius */
    float dx, dy, dz;   /* 0x28 remaining segment */
    float h, r;   /* 0x34 */
    float sg;   /* 0x3c sign of dz */
    float rr;   /* 0x40 px^2 + py^2 */
    float e;   /* 0x44 */
    float ox, oy, oz;   /* 0x48 centre of the rim circle: (0, 0, zone * h) */
    float w54, w58;   /* 0x54 */
    float ax, ay, az;   /* 0x5c axis (0, 0, 1) */
    int cnt;   /* 0x68 steps left */
    float qx, qy, qz;   /* 0x6c end point */
    float w78, w7c, w80, w84, w88, w8c;   /* 0x78 callee outputs: fraction pairs */
  } f;
  int flags, n, c, ok;
  float t, hh;
  double s, nxt, nyt, inv;

  f.cnt = 4;
  f.ax = 0.0f;
  f.ay = 0.0f;
  f.az = 1.0f;
  f.px = p[0];
  f.ox = 0.0f;
  f.py = p[1];
  f.oy = 0.0f;
  f.pz = p[2];
  f.qx = q[0];
  f.qy = q[1];
  f.qz = q[2];
  f.h = cyl->half;
  f.r = cyl->radius;
  f.e = cyl->edge;
  f.er = f.e + f.r;
  f.dx = f.qx - f.px;
  f.dy = f.qy - f.py;
  f.dz = f.qz - f.pz;
  if (f.dz < 0.0f)
    f.sg = -1.0f;
  else {
    f.sg = 1.0f;
    if (!(f.dz > 0.0f))
      f.sg = 0.0f;
  }
  if (-f.h < f.pz) {
    if (f.pz < f.h) {
      f.zone = 0.0f;
      flags = 1;
    } else {
      f.zone = 1.0f;
      flags = 0;
    }
  } else {
    f.zone = -1.0f;
    flags = 0;
  }
  f.rr = f.py * f.py + f.px * f.px;
  flags |= (f.r * f.r > f.rr) * 2;
  out->cyl = cyl;
  out->t = 0.0f;

loop:
  f.oz = f.zone * f.h;
  if (flags & 2)
    goto inside_radius;
  if (flags)
    goto inside_slab;

  /* outside both: rim torus, side quadric, cap plane */
  n = 0;
  if (f.e > 0.0f) {
    n = rwp_ray_torus_root_test(&f.tn, &f.px, &f.dx, &f.ox, &f.ax, f.r, f.e);
    if (n > 0 && f.tn == 0.0f)
      goto rim_touch;
  }
  c = rwp_line_quadric_intersect(&f.w80, &f.px, &f.dx, &f.ox, &f.ax, f.r, 0, 1);
  if (c > 0) {
    if (n <= 0 || !(f.td * f.w80 > f.w84 * f.tn)) {
      f.tn = f.w80;
      f.td = f.w84;
      flags = 2;
      n = c;
    }
  }
  if (f.zone * f.sg < 0.0f) {
    c = rwp_grid_005772c0(&f.w78, &f.px, &f.dx, 2, BITS(f.zone), BITS(f.h));
    if (c > 0) {
      if (n <= 0 || !(f.td * f.w78 > f.w7c * f.tn)) {
        f.zone = 0.0f;
        flags = 1;
        f.tn = f.w78;
        f.td = f.w7c;
        n = c;
      }
    }
  }
  if (n <= 0)
    return 0;
  if (flags == 0)
    goto torus_hit;
  goto advance;

rim_touch:
  s = 1.0 / sqrt(f.rr);
  s = s * f.r;
  f.ox = f.px * s;
  f.oy = f.py * s;
  f.nx = f.px - f.ox;
  f.ny = f.py - f.oy;
  f.nz = f.pz - f.oz;
  rwp_grid_005771f0(&f.nx, &f.nx, &f.ax, 0);
  goto done;

torus_hit:
  t = f.tn / f.td;
  out->t = t + out->t;
  f.px = f.dx * t + f.px;
  f.py = f.dy * t + f.py;
  f.pz = t * f.dz + f.pz;
  s = 1.0 / sqrt(f.py * f.py + f.px * f.px);
  s = s * f.r;
  f.oy = f.py * s;
  nxt = f.px - f.px * s;
  nyt = f.py - f.oy;
  f.nz = f.pz - f.oz;
  inv = 1.0f / f.e;
  f.er = (float)inv;
  f.nx = (float)(inv * nxt);
  f.ny = f.er * nyt;
  f.nz = f.er * f.nz;
  goto done;

inside_slab:
  /* flags == 1: inside the height slab, outside the radius */
  if (f.er * f.er >= f.rr)
    goto radial_normal;
  c = rwp_line_quadric_intersect(&f.tn, &f.px, &f.dx, &f.ox, &f.ax, f.er, 0, 0);
  if (c < 0)
    return 0;
  f.w54 = f.h - f.sg * f.pz;
  f.w58 = f.sg * f.dz;
  ok = f.w54 <= f.w58;
  if (c > 0) {
    if (ok <= 0 || !(f.w58 * f.tn > f.w54 * f.td))
      goto outer_hit;
  }
  if (ok <= 0)
    return 0;
  f.zone = f.sg;
  f.tn = f.w54;
  f.td = f.w58;
  goto advance_ebx0;

outer_hit:
  t = f.tn / f.td;
  out->t = t + out->t;
  f.px = f.dx * t + f.px;
  f.py = f.dy * t + f.py;
  f.pz = t * f.dz + f.pz;
  inv = 1.0f / f.er;
  f.nx = (float)(f.px * inv);
  f.ny = (float)(f.py * inv);
  f.nz = (float)(inv * 0.0f);
  goto done;

radial_normal:
  f.nx = f.px;
  f.ny = f.py;
  f.nz = f.pz;
  f.nz = 0.0f;
  rwp_grid_005771f0(&f.nx, &f.nx, &f.ax, 0);
  goto done;

inside_radius:
  if (flags != 2) {
    /* inside the cylinder */
    if (f.pz * f.pz > f.rr) {
      f.nx = 0.0f;
      f.ny = 0.0f;
      f.nz = f.pz < 0.0f ? -1.0f : 1.0f;
      goto done;
    }
    goto radial_normal;
  }
  /* flags == 2: inside the radius, beyond a cap */
  hh = f.e + f.h;
  n = rwp_grid_005772c0(&f.tn, &f.px, &f.dx, flags, BITS(f.zone), BITS(hh));
  if (n < 0)
    return 0;
  if (n > 0 && f.tn == 0.0f) {
    f.nx = 0.0f;
    f.ny = 0.0f;
    f.nz = f.zone;
    goto done;
  }
  c = rwp_line_quadric_intersect(&f.w88, &f.px, &f.dx, &f.ox, &f.ax, f.r, 1, 1);
  if (c > 0) {
    if (n <= 0 || !(f.td * f.w88 > f.w8c * f.tn)) {
      f.tn = f.w88;
      f.td = f.w8c;
      goto advance_ebx0;
    }
  }
  /* the cap plane is hit first */
  t = f.tn / f.td;
  f.nx = 0.0f;
  f.ny = 0.0f;
  f.nz = f.zone;
  out->t = t + out->t;
  f.px = f.dx * t + f.px;
  f.py = f.dy * t + f.py;
  f.pz = t * f.dz + f.pz;
  goto done;

advance_ebx0:
  flags = 0;
advance:
  t = f.tn / f.td;
  f.cnt--;
  out->t = t + out->t;
  f.px = f.dx * t + f.px;
  f.py = f.dy * t + f.py;
  f.pz = t * f.dz + f.pz;
  f.dx = f.qx - f.px;
  f.dy = f.qy - f.py;
  f.dz = f.qz - f.pz;
  f.rr = f.py * f.py + f.px * f.px;
  if (f.cnt != 0)
    goto loop;
  return 0;

done:
  out->pos[0] = f.px;
  out->pos[1] = f.py;
  out->pos[2] = f.pz;
  out->n[0] = f.nx;
  out->n[1] = f.ny;
  out->n[2] = f.nz;
  return 1;
}
