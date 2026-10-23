// cluster-entries: spline_object_init
/* Installed cluster spline_object_init (tools/cluster_install.py from output/cluster_pilot_pg2/spline_object_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00482270*/ extern float __cdecl spline_get_length(void *spline);
/*0048e390*/ extern void __cdecl rwp_object_set_radius_squared(void *object, float radius_squared);
/*00491530*/ extern void * __cdecl rwp_object_create_from_atomic(void *atomic, float *offset, float a, float b, float c, float d, int id);
/*004c3f60*/ extern void * __cdecl clump_first_atomic(void *clump);
/*0060d908*/ extern float g_spline_object_60d908[6];     /* default extents */
/*005dd340*/ extern float k_f_0_5;
/*005de234*/ extern float k_f_0_875;

#define SPLINE_OBJECT_MODE 9                  /* initial value of the mode word */
#define RWP_OBJECT_ID_BASE 0x75ab             /* rwp object id = base + spline object index */
#define RWP_RADIUS_SQUARED 100.0f

/* a moving object that follows a spline (record of the course spline object table) */
typedef struct SplineObject {
  void *spline;                 /* 0x00 */
  void *clump;                  /* 0x04 */
  float position;               /* 0x08: parameter along the spline */
  float speed;                  /* 0x0c */
  float limit[4];               /* 0x10: stop margins derived from the extents and the spline length */
  float extent[6];              /* 0x20: copy of g_spline_object_60d908 */
  int mode;                     /* 0x38 */
  void *rwp_object;             /* 0x3c */
  float body_offset[3];         /* 0x40: offset of the physics body from the clump (scaled 7/8 of the half extents) */
  float translation[3];         /* 0x4c: x and z stay 0, y = middle of the y extent */
} SplineObject;

/* spline_object_init: sets up spline object `obj` (spline, clump, start position, rwp physics body number `index`): speed 0,
   extents from the defaults, stop margins relative to the spline length, and the physics body from the clump's first atomic */
void spline_object_init(SplineObject *obj, void *spline, void *clump, float position, int index)
{
  double scale;
  double middle_y;

  obj->spline = spline;
  obj->clump = clump;
  obj->position = position;
  obj->speed = 0.0f;
  scale = 1.0f / spline_get_length(spline);
  obj->extent[0] = g_spline_object_60d908[0];
  obj->extent[1] = g_spline_object_60d908[1];
  obj->extent[2] = g_spline_object_60d908[2];
  obj->extent[3] = g_spline_object_60d908[3];
  obj->extent[4] = g_spline_object_60d908[4];
  obj->extent[5] = g_spline_object_60d908[5];
  obj->mode = SPLINE_OBJECT_MODE;
  obj->translation[0] = 0.0f;
  obj->translation[2] = 0.0f;
  obj->limit[0] = (float)(scale * obj->extent[2]);
  obj->limit[1] = (float)(-obj->extent[5] * scale);
  obj->limit[2] = (float)(scale * obj->extent[0]);
  obj->limit[3] = (float)(-obj->extent[3] * scale);
  middle_y = (obj->extent[1] - obj->extent[4]) * k_f_0_5;
  obj->translation[1] = (float)middle_y;
  obj->body_offset[0] = (float)((obj->extent[0] - obj->extent[3]) * k_f_0_5 * k_f_0_875);
  obj->body_offset[1] = (float)(middle_y * k_f_0_875);
  obj->body_offset[2] = (float)((obj->extent[2] - obj->extent[5]) * k_f_0_5 * k_f_0_875);
  obj->rwp_object = rwp_object_create_from_atomic(clump_first_atomic(clump), obj->body_offset, 0.0f, 0.1f, 0.9999f, 80.0f, index + RWP_OBJECT_ID_BASE);
  rwp_object_set_radius_squared(obj->rwp_object, RWP_RADIUS_SQUARED);
}

