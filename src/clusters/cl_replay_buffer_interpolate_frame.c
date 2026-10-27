// cluster-entries: replay_buffer_interpolate_frame
/* Installed cluster replay_buffer_interpolate_frame (tools/cluster_install.py from output/cluster_pilot_ql3/replay_buffer_interpolate_frame.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)
extern void *memcpy(void *, const void *, unsigned int);      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*00494630*/ extern void __cdecl hermite_interpolate(float t, float *out, float *before, float *from, float *to, float *after);
/*004d4870*/ extern RwMatrix *RwMatrixTranslate(RwMatrix *matrix, RwV3d *translation, enum RwOpCombineType combine);
/*005574a0*/ extern void __cdecl RtQuatSetupSlerpCache(float *q1, float *q2, void *cache);
/*005dd9c8*/ extern float k_f_0_16666667;
/*005dd9cc*/ extern float k_f_0_008333334;
/*005dd9d0*/ extern float k_f_1_984127en4;
/*005dd9d4*/ extern float k_f_2_7557314en6;
/*005dd9d8*/ extern float k_f_2_505076en8;
/*005dd9dc*/ extern float k_f_1_589691en10;

#define COMBINE_POSTCONCAT 2                /* rwCOMBINEPOSTCONCAT */
#define MATRIX_TYPE_ORTHONORMAL 3           /* rwMATRIXTYPEORTHONORMAL */
#define MATRIX_BYTES 0x40

/* One key frame of the recorded motion (a singly linked list): quaternion x y z w, position, next frame, time (unsigned ticks) */
typedef struct ReplayKey {
  float rotation[4];            /* 0x00 */
  RwV3d position;               /* 0x10 */
  struct ReplayKey *next;       /* 0x1c */
  unsigned int time;            /* 0x20 */
} ReplayKey;

/* The replay sampler: the five matrices it fills are the sampled pose (0x28) and the four poses the Hermite curve runs through:
   previous (0x128), current (0x68), next (0xa8) and the one after (0xe8). */
typedef struct ReplaySampler {
  char unknown_00[0x18];
  ReplayKey *keys;              /* 0x18 first key frame */
  ReplayKey *current;           /* 0x1c key frame the sampler stands on */
  char unknown_20[8];
  RwMatrix pose;                /* 0x28 */
  RwMatrix current_pose;        /* 0x68 */
  RwMatrix next_pose;           /* 0xa8 */
  RwMatrix after_pose;          /* 0xe8 */
  RwMatrix previous_pose;       /* 0x128 */
  char unknown_168[0xc];
  unsigned int time_max;        /* 0x174 */
  unsigned int time_min;        /* 0x178 */
} ReplaySampler;

/* RtQuatSlerpCache: the two quaternions to blend (the second one negated when the dot product is negative) and the angle between them */
typedef struct SlerpCache {
  float from[4];
  float to[4];
  float omega;
} SlerpCache;

/* RtQuatConvertToMatrix (RenderWare inline expansion): s = 2 / |q|^2 and the products of the scaled components, written the way the
   compiler of the original keeps them (it spills ys and the seven products wx wy wz xx yy zz yz as rounded floats, xs / zs / xy / xz
   stay in registers) */
#define QUAT_TO_MATRIX(matrix, q) \
  do { \
    float s, xs, ys, zs, wx, wy, wz, xx, yy, zz, yz, xz, xy; \
    s = 2.0f / ((q)[0] * (q)[0] + (q)[1] * (q)[1] + (q)[2] * (q)[2] + (q)[3] * (q)[3]); \
    xs = (q)[0] * s; \
    ys = s * (q)[1]; \
    zs = s * (q)[2]; \
    wx = xs * (q)[3]; \
    wy = ys * (q)[3]; \
    wz = (q)[3] * zs; \
    xx = xs * (q)[0]; \
    yy = ys * (q)[1]; \
    zz = (q)[2] * zs; \
    yz = zs * (q)[1]; \
    xz = xs * (q)[2]; \
    xy = ys * (q)[0]; \
    (matrix)->right.x = 1.0f - (zz + yy); \
    (matrix)->right.y = wz + xy; \
    (matrix)->right.z = xz - wy; \
    (matrix)->up.x = xy - wz; \
    (matrix)->up.y = 1.0f - (zz + xx); \
    (matrix)->up.z = yz + wx; \
    (matrix)->pos.x = 0.0f; \
    (matrix)->pos.y = 0.0f; \
    (matrix)->pos.z = 0.0f; \
    (matrix)->flags = MATRIX_TYPE_ORTHONORMAL; \
    (matrix)->at.x = xz + wy; \
    (matrix)->at.y = yz - wx; \
    (matrix)->at.z = 1.0f - (yy + xx); \
  } while (0)

/* the blended rotation sums its squares in another order */
#define QUAT_TO_MATRIX_D(matrix, q) \
  do { \
    float s, xs, ys, zs, wx, wy, wz, xx, yy, zz, yz, xz, xy; \
    s = 2.0f / ((q)[1] * (q)[1] + (q)[0] * (q)[0] + (q)[2] * (q)[2] + (q)[3] * (q)[3]); \
    xs = (q)[0] * s; \
    ys = s * (q)[1]; \
    zs = s * (q)[2]; \
    wx = xs * (q)[3]; \
    wy = ys * (q)[3]; \
    wz = (q)[3] * zs; \
    xx = xs * (q)[0]; \
    yy = ys * (q)[1]; \
    zz = (q)[2] * zs; \
    yz = zs * (q)[1]; \
    xz = xs * (q)[2]; \
    xy = ys * (q)[0]; \
    (matrix)->right.x = 1.0f - (zz + yy); \
    (matrix)->right.y = wz + xy; \
    (matrix)->right.z = xz - wy; \
    (matrix)->up.x = xy - wz; \
    (matrix)->up.y = 1.0f - (zz + xx); \
    (matrix)->up.z = yz + wx; \
    (matrix)->pos.x = 0.0f; \
    (matrix)->pos.y = 0.0f; \
    (matrix)->pos.z = 0.0f; \
    (matrix)->flags = MATRIX_TYPE_ORTHONORMAL; \
    (matrix)->at.x = xz + wy; \
    (matrix)->at.y = yz - wx; \
    (matrix)->at.z = 1.0f - (yy + xx); \
  } while (0)

/* replay_buffer_interpolate_frame: samples the recorded motion at `time` (ticks, clamped to the recorded range): finds the key frame
   pair around it, builds the matrices of the previous / current / next / after key frames, blends the rotation of the pair (slerp)
   and the position (Hermite through the four key frames) into the sampler's pose matrix.  When `out` is given the 16 words of the
   pose are copied there.  Before the first key frame the sampler only fills the current / next matrices. */
void __cdecl replay_buffer_interpolate_frame(ReplaySampler *sampler, unsigned int *out, unsigned int time)
{
  struct {
    struct {
      float p1[3];              /* Hermite points: current ... */
      float p2[3];              /* next */
      float p3[3];              /* after */
      float p0[3];              /* previous */
    } hermite;
    float quat[4];              /* the blended rotation */
    float alpha;                /* position between the two key frames, 0..1 */
    RwMatrix *pose;
    ReplayKey *previous_key;
    void *spill[5];
    SlerpCache cache;
    /* RESIDUE: the original reads this word (the tail `select` argument of RenderWare's inlined slerp: 0 = sine weights,
       else plain linear weights) without ever writing it.  It is the stack garbage left at that frame slot by earlier
       calls.  Zero-initialising it would force the sine weights always, which differs from the original; it is read as is
       (the diffemu uninitialised-stack rerun compares both sides over the same constant fill). */
    int select;
    RwV3d position;
  } frame;
  ReplayKey *first = sampler->keys;
  ReplayKey *key, *next, *after;
  float weight_from, weight_to, angle, s;
  float *q;

  frame.previous_key = 0;
  if (time > sampler->time_max) {
    time = sampler->time_max;
  }
  if (time < sampler->time_min) {
    time = sampler->time_min;
  }
  key = first;
  next = first->next;
  if (next != 0 && next->time < time) {
    while (key->time < time) {
      frame.previous_key = key;
      key = next;
      next = key->next;
      if (next == 0 || !(next->time < time)) {
        break;
      }
    }
  }
  sampler->current = key;
  next = key->next;
  if (next == 0) {
    ReplayKey *last = first;
    while (last->next != 0) {
      last = last->next;
    }
    QUAT_TO_MATRIX(&sampler->pose, last->rotation);
    RwMatrixTranslate(&sampler->pose, &last->position, COMBINE_POSTCONCAT);
    return;
  }
  RtQuatSetupSlerpCache(key->rotation, next->rotation, &frame.cache);
  QUAT_TO_MATRIX(&sampler->current_pose, key->rotation);
  RwMatrixTranslate(&sampler->current_pose, &key->position, COMBINE_POSTCONCAT);
  QUAT_TO_MATRIX(&sampler->next_pose, next->rotation);
  RwMatrixTranslate(&sampler->next_pose, &next->position, COMBINE_POSTCONCAT);
  if (key->time > time) {
    return;
  }
  frame.alpha = (float)(unsigned int)(time - key->time) / (float)(unsigned int)(next->time - key->time);
  if (frame.alpha <= 0.0f) {
    frame.quat[0] = key->rotation[0];
    frame.quat[1] = key->rotation[1];
    frame.quat[2] = key->rotation[2];
    frame.quat[3] = key->rotation[3];
  } else if (1.0f <= frame.alpha) {
    frame.quat[0] = next->rotation[0];
    frame.quat[1] = next->rotation[1];
    frame.quat[2] = next->rotation[2];
    frame.quat[3] = next->rotation[3];
  } else {
    weight_from = 1.0f - frame.alpha;
    weight_to = frame.alpha;
    if (frame.select == 0) {
      /* sin(x) as a Taylor series: weight = sin(t * omega) */
      angle = weight_from * frame.cache.omega;
      s = angle * angle;
      weight_from = s * angle * (((((k_f_1_589691en10 * s - k_f_2_505076en8) * s + k_f_2_7557314en6) * s - k_f_1_984127en4) * s + k_f_0_008333334) * s - k_f_0_16666667) + angle;
      angle = frame.cache.omega * frame.alpha;
      s = angle * angle;
      weight_to = s * angle * (((((k_f_1_589691en10 * s - k_f_2_505076en8) * s + k_f_2_7557314en6) * s - k_f_1_984127en4) * s + k_f_0_008333334) * s - k_f_0_16666667) + angle;
    }
    frame.quat[0] = frame.cache.to[0] * weight_to + frame.cache.from[0] * weight_from;
    frame.quat[1] = frame.cache.to[1] * weight_to + frame.cache.from[1] * weight_from;
    frame.quat[2] = frame.cache.to[2] * weight_to + frame.cache.from[2] * weight_from;
    frame.quat[3] = frame.cache.from[3] * weight_from + frame.cache.to[3] * weight_to;
  }
  frame.pose = &sampler->pose;
  q = frame.quat;
  QUAT_TO_MATRIX_D(&sampler->pose, q);
  after = next->next;
  if (after == 0) {
    QUAT_TO_MATRIX(&sampler->after_pose, next->rotation);
    RwMatrixTranslate(&sampler->after_pose, &next->position, COMBINE_POSTCONCAT);
  } else {
    QUAT_TO_MATRIX(&sampler->after_pose, after->rotation);
    RwMatrixTranslate(&sampler->after_pose, &after->position, COMBINE_POSTCONCAT);
  }
  if (frame.previous_key == 0) {
    QUAT_TO_MATRIX(&sampler->previous_pose, key->rotation);
    RwMatrixTranslate(&sampler->previous_pose, &key->position, COMBINE_POSTCONCAT);
  } else {
    QUAT_TO_MATRIX(&sampler->previous_pose, frame.previous_key->rotation);
    RwMatrixTranslate(&sampler->previous_pose, &frame.previous_key->position, COMBINE_POSTCONCAT);
  }
  frame.hermite.p0[0] = sampler->previous_pose.pos.x;
  frame.hermite.p0[1] = sampler->previous_pose.pos.y;
  frame.hermite.p0[2] = sampler->previous_pose.pos.z;
  frame.hermite.p1[0] = sampler->current_pose.pos.x;
  frame.hermite.p1[1] = sampler->current_pose.pos.y;
  frame.hermite.p1[2] = sampler->current_pose.pos.z;
  frame.hermite.p2[0] = sampler->next_pose.pos.x;
  frame.hermite.p2[1] = sampler->next_pose.pos.y;
  frame.hermite.p2[2] = sampler->next_pose.pos.z;
  frame.hermite.p3[0] = sampler->after_pose.pos.x;
  frame.hermite.p3[1] = sampler->after_pose.pos.y;
  frame.hermite.p3[2] = sampler->after_pose.pos.z;
  hermite_interpolate(frame.alpha, &frame.position.x, frame.hermite.p0, frame.hermite.p1, frame.hermite.p2, frame.hermite.p3);
  RwMatrixTranslate(frame.pose, &frame.position, COMBINE_POSTCONCAT);
  if (out != 0) {
    memcpy(out, frame.pose, MATRIX_BYTES);
  }
}

