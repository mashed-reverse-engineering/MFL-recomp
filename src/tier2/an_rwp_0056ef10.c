#include "mfl_tier2.h"

extern int RtQuatConvertFromMatrix(void *quat, void *matrix);
extern int rwp_obj_0056ebe0(float *m, float *v, int max_iterations, float eps);

typedef struct MflMat3 { float f[9]; } MflMat3;
/* A matrix as RenderWare lays it out: three rows of three floats and a pad word each. */
typedef struct MflRows3 {
  int r0[3]; int pad0;
  int r1[3]; int pad1;
  int r2[3]; int pad2;
} MflRows3;
/* Principal axes of a body: the three eigenvalues of the inertia tensor at 0, then the orientation quaternion at 0xc. */
typedef struct MflPrincipal {
  float moments[3];
  float quat[4];   /* 0xc */
} MflPrincipal;

/* Diagonalises the symmetric 3x3 inertia tensor t with the Jacobi iteration rwp_obj_0056ebe0 (25 sweeps, eps 1e-6): the eigenvalues go to
   out->moments and the eigenvector matrix (transposed into row form) is converted to the quaternion out->quat.  Returns 0 when the
   iteration failed, else 1. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_obj_0056ef10(MflPrincipal *out, MflMat3 *t)
{
  MflMat3 m, v;
  MflRows3 rows;
  int *vi = (int *)v.f;

  m = *t;
  v.f[0] = 1.0f; v.f[1] = 0.0f; v.f[2] = 0.0f;
  v.f[3] = 0.0f; v.f[4] = 1.0f; v.f[5] = 0.0f;
  v.f[6] = 0.0f; v.f[7] = 0.0f; v.f[8] = 1.0f;
  if (!rwp_obj_0056ebe0(m.f, v.f, 25, 1.0e-6f)) return 0;
  rows.r0[0] = vi[0]; rows.r0[1] = vi[3]; rows.r0[2] = vi[6];
  rows.r1[0] = vi[1]; rows.r1[1] = vi[4]; rows.r1[2] = vi[7];
  rows.r2[0] = vi[2]; rows.r2[1] = vi[5]; rows.r2[2] = vi[8];
  RtQuatConvertFromMatrix(out->quat, &rows);
  *(int *)&out->moments[0] = ((int *)m.f)[0];
  *(int *)&out->moments[1] = ((int *)m.f)[4];
  *(int *)&out->moments[2] = ((int *)m.f)[8];
  return 1;
}
