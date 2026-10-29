// cluster-entries: shadow_project_to_ground_00499dc0
/* Installed cluster shadow_project_to_ground_00499dc0 (tools/cluster_install.py from output/cluster_pilot_y1/shadow_project_to_ground_00499dc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042a440*/ extern int course_get_directional_light(void);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *frame);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *out, RwMatrix *in);
/*00557860*/ extern int __cdecl RtIntersectionLineTriangle(float *start, float *delta, const float *v0, const float *v1, const float *v2, float *t);
/*00549290*/ extern int RpCollisionWorldForAllIntersections(int world, void *intersection, void *callback, void *data);
/*00499cc0*/ extern undefined4 shadow_project_to_ground_00499dc0_cb_499cc0(float *segment, RpWorldSector *sector, float *triangle, float t, float *hit);
/*00467fd0*/ extern int __cdecl course_surface_id_is_active_check(void *material);
/*00467ef0*/ extern int __cdecl shadow_project_to_ground(RwMatrix *matrix, float *plane, RwV3d *translation);
/*00482070*/ extern float __cdecl dot_product_3d_clamped(float *a, float *b);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*004d3cc0*/ extern RwMatrix * RwMatrixUpdate(RwMatrix *matrix);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *out, struct RwV3d *in, RwInt32 n, RwMatrix *matrix);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *matrix, struct RwV3d *scale, enum RwOpCombineType combine);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*004b3284*/ extern double crt_acos_body(double x);
/*005e0968*/ extern float k_f_2_5e0968;
/*005de490*/ extern float k_f_0_02;
/*005dd338*/ extern float k_f_0_01;
/*005e64a4*/ extern float k_f_0;
/*005dd378*/ extern float k_f_0_1;
/*005ddb08*/ extern float k_f_1_4;
/*0075964c*/ extern float g_shadow_project_75964c[3];   /* direction of the directional light (its frame's "at" axis) */
#pragma intrinsic(tan)
/* a float stored and read back (the original spills these x87 values to stack slots) */
static float float_barrier(float v) { return v; }

/* A triangle of the world as the collision callback copies it (10 words); the cache of the previous hit has the same layout. */
typedef struct ShadowTriangle {
  float plane[6];          /* normal (3) + 3 more plane words */
  int index;
  const float *vertex[3];  /* pointers to the three vertices */
} ShadowTriangle;

/* The record that shadow_project_to_ground_00499dc0_cb_499cc0 fills (0x50 bytes) */
typedef struct ShadowHit {
  void *sector;
  void *material;
  float normal[3];
  float position[3];
  int found;
  ShadowTriangle triangle;
  float scaled_t;
} ShadowHit;

typedef struct ShadowRay {   /* RpIntersection of type line */
  float start[3];
  float end[3];
  int type;
} ShadowRay;

/* shadow_project_to_ground_00499dc0 */

/* "Builds the shadow matrix of an object: casts a ray from the object ray_len the directional light, finds the ground triangle (or reuses `cache`, the triangle of the
   previous cast), orients the matrix ray_len the light and scales it to the object's bounding box seen from the light. Returns 0 when there is no light or no ground." */
int __cdecl shadow_project_to_ground_00499dc0(int world, RwMatrix *object, float *bbox, RwMatrix *shadow, ShadowTriangle *cache)
{
  float light_step[3];
  ShadowRay ray;
  ShadowHit hit;
  RwMatrix inverse;
  RwMatrix *light_matrix;
  RwV3d corner[8];
  RwV3d scale;
  RwV3d look;
  float eye[3];
  float to_vertex[3];
  float t;
  float dist;
  float width;
  float min_x, max_x, min_y, max_y;
  int light;
  int i;
  double light_dot_plane;
  double ray_len;
  double shadow_width;

  light_step[0] = 0.0f;
  light_step[1] = 0.0f;
  light_step[2] = 0.0f;
  light = course_get_directional_light();
  if (light == 0 || world == 0) {
    return 0;
  }
  light_matrix = RwFrameGetLTM(*(struct RwFrame **)(light + 4));
  RwMatrixInvert(&inverse, light_matrix);
  g_shadow_project_75964c[0] = light_matrix->at.x;
  light_step[0] = k_f_2_5e0968 * g_shadow_project_75964c[0];
  g_shadow_project_75964c[1] = light_matrix->at.y;
  g_shadow_project_75964c[2] = light_matrix->at.z;
  light_step[1] = g_shadow_project_75964c[1] * k_f_2_5e0968;
  ray.start[0] = object->pos.x;
  light_step[2] = g_shadow_project_75964c[2] * k_f_2_5e0968;
  ray.start[1] = object->pos.y;
  ray.start[2] = object->pos.z;
  ray.end[0] = light_step[0] + object->pos.x;
  ray.end[1] = object->pos.y + light_step[1];
  hit.found = 0;
  ray.end[2] = object->pos.z + light_step[2];
  ray.type = 1;
  if (cache != 0 && (t = 0.0f, cache->index >= 0) && cache->vertex[0] != 0 && cache->vertex[1] != 0 && cache->vertex[2] != 0
      && RtIntersectionLineTriangle(ray.start, light_step, cache->vertex[0], cache->vertex[1], cache->vertex[2], &t) != 0) {
    /* the previous ground triangle still lies under the object */
    hit.normal[0] = cache->plane[0];
    hit.normal[1] = cache->plane[1];
    hit.normal[2] = cache->plane[2];
    hit.found = 1;
    /* hit point = ray point at t, pushed 2 % of the normal off the triangle (the original stores the first sum, then adds the push) */
    hit.position[0] = float_barrier(float_barrier((ray.end[0] - ray.start[0]) * t) + ray.start[0]);
    hit.position[1] = float_barrier((ray.end[1] - ray.start[1]) * t + ray.start[1]);
    hit.position[0] = cache->plane[0] * k_f_0_02 + hit.position[0];
    hit.position[1] = cache->plane[1] * k_f_0_02 + k_f_0_01 + hit.position[1];
    hit.position[2] = float_barrier(cache->plane[2] * k_f_0_02) + (float_barrier(ray.end[2] - ray.start[2]) * t + ray.start[2]);
    hit.scaled_t = t * k_f_2_5e0968;
    hit.triangle = *cache;
  } else {
    RpCollisionWorldForAllIntersections(world, &ray, shadow_project_to_ground_00499dc0_cb_499cc0, &hit);
    if (hit.found == 0 || course_surface_id_is_active_check(hit.material) != 0) {
      return 0;
    }
    if (cache != 0) {
      *cache = hit.triangle;
    }
  }

  /* the matrix starts as the identity ... */
  shadow->right.x = 1.0f;
  shadow->up.y = 1.0f;
  shadow->at.z = 1.0f;
  shadow->up.x = 0.0f;
  shadow->right.z = 0.0f;
  shadow->right.y = 0.0f;
  shadow->at.y = 0.0f;
  shadow->at.x = 0.0f;
  shadow->up.z = 0.0f;
  shadow->pos.z = 0.0f;
  shadow->pos.y = 0.0f;
  shadow->pos.x = 0.0f;
  shadow->flags |= 0x20003;
  /* ... and is oriented by the ground plane, then turned so that "right" points from the hit to the light's view of the ground vertex */
  shadow_project_to_ground(shadow, hit.triangle.plane, 0);
  eye[0] = float_barrier(light_matrix->up.x + hit.position[0]);
  eye[1] = float_barrier(light_matrix->up.y + hit.position[1]);
  eye[2] = float_barrier(light_matrix->up.z + hit.position[2]);
  to_vertex[0] = hit.triangle.vertex[0][0] - eye[0];
  to_vertex[1] = hit.triangle.vertex[0][1] - eye[1];
  to_vertex[2] = hit.triangle.vertex[0][2] - eye[2];
  dist = dot_product_3d_clamped(hit.triangle.plane, to_vertex);
  light_dot_plane = dot_product_3d_clamped(hit.triangle.plane, g_shadow_project_75964c);
  if (light_dot_plane == k_f_0) {
    light_dot_plane = k_f_0_01;
  }
  ray_len = (double)dist / light_dot_plane;
  look.x = (RwReal)((g_shadow_project_75964c[0] * ray_len + eye[0]) - hit.position[0]);
  look.y = float_barrier(ray_len * g_shadow_project_75964c[1] + eye[1]) - hit.position[1];
  look.z = float_barrier(ray_len * g_shadow_project_75964c[2] + eye[2]) - hit.position[2];
  RwV3dNormalize(&shadow->right, &look);
  shadow->up.x = shadow->at.y * shadow->right.z - shadow->right.y * shadow->at.z;
  shadow->up.y = shadow->at.z * shadow->right.x - shadow->at.x * shadow->right.z;
  shadow->up.z = shadow->at.x * shadow->right.y - shadow->at.y * shadow->right.x;
  RwMatrixUpdate(shadow);

  /* the eight corners of the bounding box in the light's frame */
  corner[0].x = bbox[3]; corner[0].y = bbox[4]; corner[0].z = bbox[5];
  corner[1].x = bbox[0]; corner[1].y = bbox[4]; corner[1].z = bbox[5];
  corner[2].x = bbox[0]; corner[2].y = bbox[1]; corner[2].z = bbox[5];
  corner[3].x = bbox[3]; corner[3].y = bbox[1]; corner[3].z = bbox[5];
  corner[4].x = bbox[3]; corner[4].y = bbox[4]; corner[4].z = bbox[2];
  corner[5].x = bbox[0]; corner[5].y = bbox[4]; corner[5].z = bbox[2];
  corner[6].x = bbox[0]; corner[6].y = bbox[1]; corner[6].z = bbox[2];
  corner[7].x = bbox[3]; corner[7].y = bbox[1]; corner[7].z = bbox[2];
  RwV3dTransformVectors(corner, corner, 8, object);
  RwV3dTransformVectors(corner, corner, 8, &inverse);
  min_y = 1000.0f;
  max_y = -1000.0f;
  min_x = 1000.0f;
  max_x = -1000.0f;
  for (i = 0; i < 8; i++) {
    if (max_y < corner[i].y) max_y = corner[i].y;
    if (corner[i].y < min_y) min_y = corner[i].y;
    if (max_x < corner[i].x) max_x = corner[i].x;
    if (corner[i].x < min_x) min_x = corner[i].x;
  }
  width = g_shadow_project_75964c[2] * shadow->right.z + g_shadow_project_75964c[0] * shadow->right.x + g_shadow_project_75964c[1] * shadow->right.y;
  max_y = max_y - min_y;
  shadow_width = max_y / tan(crt_acos_body(width));
  scale.z = 1.0f;
  width = (float)shadow_width;
  scale.x = (RwReal)shadow_width;
  scale.y = (max_x - min_x) * k_f_1_4;
  RwMatrixScale(shadow, &scale, rwCOMBINEPRECONCAT);
  t = width * k_f_0_1;
  look.y = t * shadow->right.y;
  look.z = t * shadow->right.z;
  hit.position[0] = hit.position[0] + t * shadow->right.x;
  hit.position[1] = hit.position[1] + look.y;
  hit.position[2] = hit.position[2] + look.z;
  RwMatrixTranslate(shadow, (RwV3d *)hit.position, rwCOMBINEPOSTCONCAT);
  return 1;
}

