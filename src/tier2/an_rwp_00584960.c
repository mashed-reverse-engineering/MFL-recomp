#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern float rwp_sphere_0056c1c0(char *body, float *props, float *centre);
extern int rwp_obj_0056ea00(float *dst, float *prop);
extern void rwp_aggregate_00584f40(float *acc, float *v, float scale);
extern int rwp_obj_0056ef10(float *out, float *m);

/* rwp_aggregate_00584960: mass properties of a compound of n bodies (bodies = array of 0x60 byte frames, rows right/up/at at +0/+0x10/+0x20).
   Per body 0056c1c0 returns its mass and fills the 7 float property block (3 moments, quaternion; 0x1c bytes each) and its centre.
   Result: *out = principal moments (3 floats) + orientation quaternion (4) of the summed inertia tensor about the centre of mass, returns the total mass
   (0 and *out = 1,1,1,0,0,0,1 when it is <= FLT_MIN).  com_given == 0: com receives the mass weighted centre (0 first); else com is an input. */
/* r = a1*a2 + b1*b2 + c1*c2, each product and sum in the written order (cl would regroup a plain expression) */
#define SUM3(r, a1, a2, b1, b2, c1, c2) do { (r) = (a1) * (a2); t_ = (b1) * (b2); (r) = (r) + t_; t_ = (c1) * (c2); (r) = (r) + t_; } while (0)
#define ROW(b, k, j) (*(float *)((char *)(b) + 0x10 * (k) + 4 * (j)))      /* body frame: row k (right, up, at), component j */

/* TIER2: unvalidated 2026-11-02 authority=diffemu */
float rwp_aggregate_00584960(char *bodies, unsigned int n, float *out, float *com, int com_given)
{
  float total = 0.0f;
  float acc[9];      /* summed inertia tensor, row major */
  float T[9], N[9], W[9], P[9];
  float *ctr, *mass, *props;
  float *c, *pr;
  char *b;
  float m, inv, w, t_;
  double dx, dy, dz, d2, td;      /* x87 register values of the original (extended exponent range) */
  unsigned int i;

  acc[0] = acc[1] = acc[2] = acc[3] = acc[4] = acc[5] = acc[6] = acc[7] = acc[8] = 0.0f;
  if (com_given == 0) { com[0] = 0.0f; com[1] = 0.0f; com[2] = 0.0f; }
  ctr = (float *)RwMalloc(12 * n, 0x10900);
  mass = (float *)RwMalloc(4 * n, 0x10900);
  props = (float *)RwMalloc(28 * n, 0x10900);
  c = ctr; pr = props; b = bodies;
  for (i = 0; i < n; i++) {
    m = rwp_sphere_0056c1c0(b, pr, c);
    mass[i] = m;
    total = total + m;
    if (com_given == 0) {
      com[0] = m * c[0] + com[0];
      com[1] = c[1] * mass[i] + com[1];
      com[2] = mass[i] * c[2] + com[2];
    }
    c += 3; pr += 7; b += 0x60;
  }
  if (n == 0 || total <= 1.17549435e-38f) {
    out[0] = 1.0f; out[1] = 1.0f; out[2] = 1.0f;
    out[3] = 0.0f; out[4] = 0.0f; out[5] = 0.0f; out[6] = 1.0f;
    total = 0.0f;
  } else {
    if (com_given == 0) {
      inv = 1.0f / total;
      com[0] = com[0] * inv; com[1] = com[1] * inv; com[2] = com[2] * inv;
    }
    c = ctr; pr = props; b = bodies;
    for (i = 0; i < n; i++, c += 3, pr += 7, b += 0x60) {
      if (mass[i] > 0.0f) {
        rwp_obj_0056ea00(T, pr);
        SUM3(N[0], T[1], ROW(b, 1, 0), T[2], ROW(b, 2, 0), T[0], ROW(b, 0, 0));
        SUM3(N[1], T[2], ROW(b, 2, 1), T[1], ROW(b, 1, 1), T[0], ROW(b, 0, 1));
        SUM3(N[2], T[2], ROW(b, 2, 2), T[1], ROW(b, 1, 2), T[0], ROW(b, 0, 2));
        SUM3(N[3], T[5], ROW(b, 2, 0), T[3], ROW(b, 0, 0), T[4], ROW(b, 1, 0));
        SUM3(N[4], T[5], ROW(b, 2, 1), T[4], ROW(b, 1, 1), T[3], ROW(b, 0, 1));
        SUM3(N[5], T[5], ROW(b, 2, 2), T[4], ROW(b, 1, 2), T[3], ROW(b, 0, 2));
        SUM3(N[6], T[8], ROW(b, 2, 0), T[6], ROW(b, 0, 0), T[7], ROW(b, 1, 0));
        SUM3(N[7], T[8], ROW(b, 2, 1), T[7], ROW(b, 1, 1), T[6], ROW(b, 0, 1));
        SUM3(N[8], T[8], ROW(b, 2, 2), T[7], ROW(b, 1, 2), T[6], ROW(b, 0, 2));
        SUM3(W[0], N[3], ROW(b, 1, 0), N[6], ROW(b, 2, 0), N[0], ROW(b, 0, 0));
        SUM3(W[1], N[6], ROW(b, 2, 1), N[3], ROW(b, 1, 1), N[0], ROW(b, 0, 1));
        SUM3(W[2], N[6], ROW(b, 2, 2), N[3], ROW(b, 1, 2), N[0], ROW(b, 0, 2));
        SUM3(W[3], N[4], ROW(b, 1, 0), N[1], ROW(b, 0, 0), N[7], ROW(b, 2, 0));
        SUM3(W[4], N[1], ROW(b, 0, 1), N[7], ROW(b, 2, 1), N[4], ROW(b, 1, 1));
        SUM3(W[5], N[1], ROW(b, 0, 2), N[7], ROW(b, 2, 2), N[4], ROW(b, 1, 2));
        SUM3(W[6], N[5], ROW(b, 1, 0), N[8], ROW(b, 2, 0), N[2], ROW(b, 0, 0));
        SUM3(W[7], N[8], ROW(b, 2, 1), N[5], ROW(b, 1, 1), N[2], ROW(b, 0, 1));
        SUM3(W[8], N[8], ROW(b, 2, 2), N[5], ROW(b, 1, 2), N[2], ROW(b, 0, 2));
        t_ = (W[3] + W[1]) * 0.5f; W[1] = t_; W[3] = t_;
        t_ = (W[2] + W[6]) * 0.5f; W[2] = t_; W[6] = t_;
        t_ = (W[7] + W[5]) * 0.5f; W[7] = t_; W[5] = t_;
        inv = 1.0f / total;
        rwp_aggregate_00584f40(acc, W, inv * mass[i]);
        dx = c[0] - com[0]; dy = c[1] - com[1]; dz = c[2] - com[2];
        d2 = dy * dy; d2 = -d2;
        td = dz * dz; P[0] = d2 - td;
        td = dx * dx; P[4] = -td - (dz * dz);
        P[8] = d2 - td;
        P[1] = P[3] = dy * dx;
        P[2] = P[6] = dz * dx;
        P[5] = P[7] = dz * dy;
        rwp_aggregate_00584f40(acc, P, -(inv * mass[i]));
      }
    }
    rwp_obj_0056ef10(out, acc);
  }
  RwFree(ctr);
  RwFree(mass);
  RwFree(props);
  return total;
}
