#include "mfl_tier2.h"

typedef struct MflVel { float lin[3], pad3, ang[3], pad7; } MflVel;   /* 32 bytes */
typedef struct MflXf { float m[16]; } MflXf;   /* 64 bytes */
typedef struct MflBodySet {
  MflXf *xf;   /* 0x0 */
  int pad4;
  MflVel *vel;   /* 0x8 */
} MflBodySet;
typedef struct MflWorld {
  int pad00[4];
  MflBodySet *bodies;   /* 0x10 */
} MflWorld;
typedef struct MflBodyHandle {
  MflWorld *world;
  int index;
} MflBodyHandle;

/* Velocity of the point p of a body: out = linear + angular x (p - position). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_sphere_0056bd20(MflBodyHandle *h, float *p, float *out)
{
  float cross[2];
  double dx, dy, dz, cz;
  MflBodySet *set = h->world->bodies;
  float *m = set->xf[h->index].m;
  MflVel *v;

  dx = p[0] - m[12];
  dy = p[1] - m[13];
  dz = p[2] - m[14];
  v = &set->vel[h->index];
  cross[0] = v->ang[1] * dz - v->ang[2] * dy;
  cross[1] = v->ang[2] * dx - v->ang[0] * dz;
  cz = v->ang[0] * dy - v->ang[1] * dx;
  out[0] = v->lin[0] + cross[0];
  out[1] = h->world->bodies->vel[h->index].lin[1] + cross[1];
  out[2] = h->world->bodies->vel[h->index].lin[2] + cz;
}
