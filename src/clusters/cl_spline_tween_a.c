// cluster-entries: spline_tween_a spline_tween_b
/* Installed cluster spline_tween_a (tools/cluster_install.py from output/cluster_pilot_pg/spline_tween_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00484c20*/ extern void __cdecl fatal_assert_loop(const char *message);
/*00494630*/ extern void __cdecl hermite_interpolate(float t, float *out, float *before, float *from, float *to, float *after);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*00542fe0*/ extern void __cdecl FUN_00542fe0(RwMatrix *out, void *key_frame);
/*005574a0*/ extern void __cdecl RtQuatSetupSlerpCache(float *q1, float *q2, void *cache);
/*005dd9c8*/ extern float k_f_0_16666667;
/*005dd9cc*/ extern float k_f_0_008333334;
/*005dd9d0*/ extern float k_f_1_984127en4;
/*005dd9d4*/ extern float k_f_2_7557314en6;
/*005dd9d8*/ extern float k_f_2_505076en8;
/*005dd9dc*/ extern float k_f_1_589691en10;

/* RtAnimAnimation / RtAnimKeyFrame layout of the key frame spline the tween samples (stride 0x24) */
typedef struct KeyFrame {
  struct KeyFrame *prev_frame;   /* 0x00 */
  float time;                    /* 0x04 */
  float rotation[4];             /* 0x08: quaternion x y z w */
  float position[3];             /* 0x18 */
} KeyFrame;

typedef struct KeyAnimation {
  void *interp_info;             /* 0x00 */
  int frame_count;               /* 0x04 */
  int flags;                     /* 0x08 */
  float duration;                /* 0x0c: time of the last frame */
  KeyFrame *frames;              /* 0x10 */
} KeyAnimation;

/* RtQuatSlerpCache: the two quaternions to blend (the second one negated when the dot product is negative) and the angle between them */
typedef struct SlerpCache {
  float from[4];
  float to[4];
  float omega;
} SlerpCache;

#define MATRIX_TYPE_ORTHONORMAL 3          /* rwMATRIXTYPEORTHONORMAL */
#define COMBINE_POSTCONCAT 2               /* rwCOMBINEPOSTCONCAT */

/*
 * Samples the key frame spline `anim` at time `t` into the matrix `out`: rotation = normalised slerp between the two
 * frames around `t`, translation = Hermite spline through four frames.  Before the first / after the last frame the
 * first / last frame is applied unchanged.  `wrap` makes the spline cyclic: the neighbour before the first frame is
 * the last one and the one after the last is the first (spline_tween_b); otherwise the end frames repeat (spline_tween_a).
 */
static void spline_tween(RwMatrix *out, KeyAnimation *anim, float t, int wrap)
{
  KeyFrame *frames = anim->frames;
  KeyFrame *before;
  KeyFrame *from;
  KeyFrame *to;
  KeyFrame *after;
  int count = anim->frame_count;
  int next;
  struct {
    SlerpCache cache;
    /* RESIDUE: the original reads this word (the tail `select` argument of RenderWare's inlined slerp: 0 = sine weights,
       else plain linear weights) without ever writing it.  It is the stack garbage left at that frame slot by earlier
       calls, nonzero in practice.  Zero-initialising it would force the sine weights always, which differs from the
       original on every harness fill except 0x00, so it is read as is (the diffemu uninitialised-stack rerun compares
       both sides over the same constant fill). */
    int select;
  } slerp;
  float u;
  float q[4];
  float position[3];
  float weight_from;
  float weight_to;
  float angle;
  float s, xs, ys, zs, wx, wy, wz, xx, yy, zz, xy, xz, yz;

  if (t <= 0.0f) {
    FUN_00542fe0(out, frames);
    return;
  }
  if (t >= anim->duration) {
    FUN_00542fe0(out, &frames[count - 1]);
    return;
  }
  for (next = 1; t > frames[next].time; next++) {
  }
  from = &frames[next - 1];
  to = &frames[next];
  before = next > 1 ? &frames[next - 2] : wrap ? &frames[count - 1] : frames;
  after = next < count - 2 ? &frames[next + 1] : wrap ? frames : &frames[next];
  u = (t - from->time) / (to->time - from->time);
  if (u < 0.0f) {
    fatal_assert_loop("tween is less than 0.0f!!\n");
  }
  if (u > 1.0f) {
    fatal_assert_loop("tween is greater than 1.0f!!\n");
  }
  RtQuatSetupSlerpCache(from->rotation, to->rotation, &slerp.cache);
  if (u <= 0.0f) {
    q[0] = from->rotation[0];
    q[1] = from->rotation[1];
    q[2] = from->rotation[2];
    q[3] = from->rotation[3];
  } else if (u >= 1.0f) {
    q[0] = to->rotation[0];
    q[1] = to->rotation[1];
    q[2] = to->rotation[2];
    q[3] = to->rotation[3];
  } else {
    weight_from = 1.0f - u;
    weight_to = u;
    if (slerp.select == 0) {
      /* sin(x) as a Taylor series: weight = sin(t * omega) */
      angle = weight_from * slerp.cache.omega;
      s = angle * angle;
      weight_from = s * angle * (((((k_f_1_589691en10 * s - k_f_2_505076en8) * s + k_f_2_7557314en6) * s - k_f_1_984127en4) * s + k_f_0_008333334) * s - k_f_0_16666667) + angle;
      angle = slerp.cache.omega * u;
      s = angle * angle;
      weight_to = s * angle * (((((k_f_1_589691en10 * s - k_f_2_505076en8) * s + k_f_2_7557314en6) * s - k_f_1_984127en4) * s + k_f_0_008333334) * s - k_f_0_16666667) + angle;
    }
    q[0] = slerp.cache.to[0] * weight_to + slerp.cache.from[0] * weight_from;
    q[1] = slerp.cache.to[1] * weight_to + slerp.cache.from[1] * weight_from;
    q[2] = slerp.cache.to[2] * weight_to + slerp.cache.from[2] * weight_from;
    q[3] = slerp.cache.from[3] * weight_from + slerp.cache.to[3] * weight_to;
  }
  hermite_interpolate(u, position, before->position, from->position, to->position, after->position);
  /* quaternion (x y z w) to rotation matrix; s = 2 / |q|^2 */
  s = 2.0f / (q[1] * q[1] + q[0] * q[0] + q[2] * q[2] + q[3] * q[3]);
  xs = q[0] * s;
  ys = s * q[1];
  zs = s * q[2];
  wx = xs * q[3];
  wy = ys * q[3];
  wz = q[3] * zs;
  xx = xs * q[0];
  yy = ys * q[1];
  zz = q[2] * zs;
  yz = zs * q[1];
  xz = xs * q[2];
  xy = ys * q[0];
  out->right.x = 1.0f - (zz + yy);
  out->right.y = wz + xy;
  out->right.z = xz - wy;
  out->flags = MATRIX_TYPE_ORTHONORMAL;
  out->up.x = xy - wz;
  out->up.y = 1.0f - (zz + xx);
  out->up.z = yz + wx;
  out->at.x = xz + wy;
  out->at.y = yz - wx;
  out->at.z = 1.0f - (yy + xx);
  out->pos.x = 0.0f;
  out->pos.y = 0.0f;
  out->pos.z = 0.0f;
  RwMatrixTranslate(out, (struct RwV3d *)position, COMBINE_POSTCONCAT);
}

/* spline_tween_a (00405e50): the end frames repeat */
/* TIER2: unvalidated 2026-10-23 authority=static */
void spline_tween_a(RwMatrix *out, KeyAnimation *anim, float t)
{
  spline_tween(out, anim, t, 0);
}

/* spline_tween_b (00484c40): cyclic spline, the neighbours of the end frames wrap around */
/* TIER2: unvalidated 2026-10-23 authority=static */
void spline_tween_b(RwMatrix *out, KeyAnimation *anim, float t)
{
  spline_tween(out, anim, t, 1);
}

