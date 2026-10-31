#include "mfl_tier2.h"

typedef struct RwpFace { unsigned char pad00[0x10]; float n[3]; unsigned char pad1c[0x14]; float p[3]; unsigned char pad3c[0xc]; unsigned short kind; } RwpFace;
typedef struct RwpFaceOwner { unsigned char pad00[8]; RwpFace *face; unsigned char pad0c[4]; float *xf; } RwpFaceOwner;
/*0056c8a0*/ extern int rwp_sphere_0056c8a0(void *a, void *b, float *dir, float *lo, float *hi);

/* Direction from the face point vector p rotated by the owner's matrix (transposed product, as the original reads it);
   *kind receives the face kind.  For kinds other than 0xe, unless bit 0 of *kind is set, the direction is flipped when the
   plane offset n.p plus the owner's translation along dir lies nearer the far end of the extent returned by the shape query. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggregate_00586200(RwpFaceOwner *o, RwpFaceOwner *other, float *dir, unsigned short *kind)
{
  RwpFace *f = o->face;
  float *m;
  float lo, hi;
  double acc;

  *kind = f->kind;
  if (f->kind != 0xe) {
    m = o->xf;
    acc = m[8] * f->p[2];
    acc = acc + m[4] * f->p[1];
    acc = acc + m[0] * f->p[0];
    dir[0] = acc;
    m = o->xf;
    acc = m[9] * f->p[2];
    acc = acc + m[1] * f->p[0];
    acc = acc + m[5] * f->p[1];
    dir[1] = acc;
    m = o->xf;
    acc = m[10] * f->p[2];
    acc = acc + m[2] * f->p[0];
    acc = acc + m[6] * f->p[1];
    dir[2] = acc;
    if (!(*kind & 1)) {
      rwp_sphere_0056c8a0(*(void **)((char *)other + 8), *(void **)((char *)other + 0x10), dir, &hi, &lo);
      m = o->xf;
      if ((f->n[0] * f->p[0] + f->n[1] * f->p[1] + f->n[2] * f->p[2] + (m[13] * dir[1] + m[12] * dir[0] + m[14] * dir[2]) - hi) > (lo - (f->n[0] * f->p[0] + f->n[1] * f->p[1] + f->n[2] * f->p[2] + (m[13] * dir[1] + m[12] * dir[0] + m[14] * dir[2])))) {
        dir[0] = -dir[0];
        dir[1] = -dir[1];
        dir[2] = -dir[2];
      }
    }
  }
}
