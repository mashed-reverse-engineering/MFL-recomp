/* rwa_soft_ray_hit_005d7d60 (0x005d7d60): ray test of a segment start..end against the object's geometry (+0x4c): fills a hit record {object, point, normal, t}; x87 */
#include "mfl_tier2.h"

/*00577350*/ extern int FUN_00577350(float *ratio, const float *start, float *dir, float *normal, void *geometry);      /* > 0: hit (ratio = {num, den}) */

typedef struct RwaRayHit {
  char *object;
  float point[3];
  float normal[3];
  float t;
} RwaRayHit;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_ray_hit_005d7d60(char *object, const float *start, const float *end, RwaRayHit *hit)
{
  float ratio[2], dir[3], normal[3], t;

  dir[0] = end[0] - start[0];
  normal[0] = 0.0f;
  normal[1] = 0.0f;
  dir[1] = end[1] - start[1];
  normal[2] = 1.0f;
  dir[2] = end[2] - start[2];
  if (FUN_00577350(ratio, start, dir, normal, *(void **)(object + 0x4c)) <= 0)
    return 0;
  t = ratio[0] / ratio[1];
  hit->object = object;
  hit->t = t;
  hit->point[0] = dir[0] * t + start[0];
  hit->point[1] = dir[1] * t + start[1];
  hit->point[2] = t * dir[2] + start[2];
  hit->normal[0] = normal[0];
  hit->normal[1] = normal[1];
  hit->normal[2] = normal[2];
  return 1;
}
