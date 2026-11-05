/* rwp_box_0056ce70 (0x0056ce70): RwpBox vtable slot (5f64ec): mass properties of a rounded box (box half sizes + margin) */
#include "mfl_tier2.h"

/* Half sizes hx, hy, hz at +0x40/+0x44/+0x48, margin r at +0x4c.  Without a margin: volume 8 hx hy hz, inertia diagonal
   (hy^2 + hz^2) / 3 ...  With a margin the volume and the inertia diagonal get the face slabs (r * area), edge cylinders and corner
   spheres of the rounded box (pi, 4/3 pi, pi/6 terms).  inertia[3..6] = identity rotation, centre = origin.  Returns the volume.
   Every intermediate the original keeps in an x87 register is a double here (exact at 24 bit precision, same exponent range for
   these products); the values it stores to its frame are floats.  One operation per statement: the original's operand order. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
double rwp_box_0056ce70(float *box, float *inertia, float *centre)
{
  float hx, hy, hz, r;
  float yz, xy, s_yz, s_xz, s_xy, b3, c3, r2a, r2b, inv;
  double vol, a, b, c, a3, ar, br, cr, t, u;
  double zz, yy, xx;

  hx = box[16];
  hz = box[18];
  hy = box[17];
  r = box[19];
  t = (double)hz * hy;
  yz = (float)t;
  vol = t * hx;
  vol = vol * 8.0f;
  zz = (double)hz * hz;
  yy = (double)hy * hy;
  s_yz = (float)(yy + zz);
  xx = (double)hx * hx;
  s_xz = (float)(xx + zz);
  s_xy = (float)(xx + yy);
  if (r > 0.0f) {
    a = (double)hz + hy;
    b = (double)hy + hx;
    xy = (float)((double)hy * hx);
    /* volume: face slabs and edge cylinders / corner spheres */
    t = a + hx;
    t = t + t;
    t = t + (double)r * 1.3333334f;
    t = t * r;
    t = t * 3.1415927f;
    u = b * hz;
    u = u + xy;
    u = u * 8.0f;
    t = t + u;
    t = t * r;
    vol = vol + t;
    u = b * b;
    u = u * b;
    b3 = (float)u;
    a3 = a * a;
    a3 = a3 * a;
    c = (double)hz + hx;
    u = c * c;
    u = u * c;
    c3 = (float)u;
    ar = a * r;
    t = (double)r * r;
    r2a = (float)(0.4f * t);
    r2b = (float)(t * 0.5f);
    inv = (float)(1.0f / vol);
    /* x */
    t = (double)r * ar;
    t = t * 3.0f;
    u = a3 * 4.0f;
    t = t + u;
    t = t * 0.5235988f;
    u = (double)r2a + s_yz;
    u = u * r;
    u = u * 4.1887903f;
    t = t + u;
    t = t * r;
    u = (double)yz * 6.0f;
    u = u + ar;
    u = u * r;
    u = u + a3;
    u = u * hx;
    u = u * 2.6666667f;
    t = t + u;
    u = (double)r2b + s_yz;
    u = u * r;
    u = u * hx;
    u = u * 6.2831855f;
    t = t + u;
    t = t * r;
    u = (double)r + hx;
    u = u * s_yz;
    u = u * hz;
    u = u * hy;
    u = u * 2.6666667f;
    t = t + u;
    t = t * inv;
    inertia[0] = (float)t;
    /* y */
    cr = c * r;
    t = (double)r * cr;
    t = t * 3.0f;
    u = (double)c3 * 4.0f;
    t = t + u;
    t = t * 0.5235988f;
    u = (double)r2a + s_xz;
    u = u * r;
    u = u * 4.1887903f;
    t = t + u;
    t = t * r;
    u = (double)hz * hx;
    u = u * 6.0f;
    u = u + cr;
    u = u * r;
    u = u + c3;
    u = u * hy;
    u = u * 2.6666667f;
    t = t + u;
    u = (double)r2b + s_xz;
    u = u * r;
    u = u * hy;
    u = u * 6.2831855f;
    t = t + u;
    t = t * r;
    u = (double)r + hy;
    u = u * s_xz;
    u = u * hz;
    u = u * hx;
    u = u * 2.6666667f;
    t = t + u;
    t = t * inv;
    inertia[1] = (float)t;
    /* z */
    br = b * r;
    t = (double)r * br;
    t = t * 3.0f;
    u = (double)b3 * 4.0f;
    t = t + u;
    t = t * 0.5235988f;
    u = (double)r2a + s_xy;
    u = u * r;
    u = u * 4.1887903f;
    t = t + u;
    t = t * r;
    u = (double)xy * 6.0f;
    u = u + br;
    u = u * r;
    u = u + b3;
    u = u * hz;
    u = u * 2.6666667f;
    t = t + u;
    u = (double)r2b + s_xy;
    u = u * r;
    u = u * hz;
    u = u * 6.2831855f;
    t = t + u;
    t = t * r;
    u = (double)r + hz;
    u = u * s_xy;
    u = u * hy;
    u = u * hx;
    u = u * 2.6666667f;
    t = t + u;
    t = t * inv;
    inertia[2] = (float)t;
  } else {
    inertia[0] = s_yz * 0.33333334f;
    inertia[1] = s_xz * 0.33333334f;
    inertia[2] = s_xy * 0.33333334f;
  }
  inertia[6] = 1.0f;
  inertia[3] = 0.0f;
  inertia[4] = 0.0f;
  inertia[5] = 0.0f;
  centre[2] = 0.0f;
  centre[1] = 0.0f;
  centre[0] = 0.0f;
  return vol;
}
