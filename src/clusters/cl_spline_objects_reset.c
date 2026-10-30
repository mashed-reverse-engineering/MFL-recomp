// cluster-entries: spline_objects_reset spline_objects_destroy
/* Installed cluster spline_objects_reset (tools/cluster_install.py from output/cluster_pilot_te5/spline_objects_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00457170*/ extern void __cdecl spline_object_update(void *obj, float dt);
/*004573d0*/ extern void __cdecl spline_object_set_position(void *obj, float position);
/*0048e720*/ extern void __cdecl rwp_object_set_matrix(void *object, RwMatrix *matrix);
/*00492d10*/ extern void __cdecl rwp_object_set_active(void *object, int active);
/*004573f0*/ extern void __cdecl spline_object_destroy(void *obj);
/*004d4870*/ extern RwMatrix * __cdecl RwMatrixTranslate(RwMatrix *matrix, const RwV3d *translation, RwOpCombineType combineOp);

#define SPLINE_OBJECT_COUNT 3

/* start of every RenderWare object: `parent` of a clump is its frame */
typedef struct ObjectHeader {
  unsigned char type, sub_type, flags, private_flags;
  void *parent;
} ObjectHeader;

/* start of a RenderWare frame: object header, dirty list link, then the modelling matrix */
typedef struct FrameHead {
  unsigned char object[8];
  unsigned char dirty_link[8];
  RwMatrix modelling;
} FrameHead;

/* a moving object that follows a spline (record of the course spline object table, stride 0x58; the leading fields as in spline_object_init) */
typedef struct SplineObject {
  void *spline;                 /* 0x00 */
  ObjectHeader *clump;          /* 0x04 */
  float position;               /* 0x08 */
  float speed;                  /* 0x0c */
  unsigned char unknown_10[0x2c];
  void *rwp_object;             /* 0x3c */
  unsigned char unknown_40[0xc];
  RwV3d translation;            /* 0x4c */
  
} SplineObject;
/*008c5500*/ extern unsigned char g_course_spline_8c5500[SPLINE_OBJECT_COUNT][0x58];
/*0060d8fc*/ extern float g_course_spline_60d8fc[SPLINE_OBJECT_COUNT];      /* start position of each object */

/* spline_objects_reset: puts every spline object back on its start position and moves the physics body (inactive) to the clump's frame matrix plus the body translation
   (called through the 5 byte jump course_tierra_n_cb6) */
void spline_objects_reset(void)
{
  int i;
  SplineObject *obj;
  RwMatrix matrix;

  for (i = 0; i < SPLINE_OBJECT_COUNT; i++) {
    obj = (SplineObject *)g_course_spline_8c5500[i];
    spline_object_set_position(obj, g_course_spline_60d8fc[i]);
    spline_object_update(obj, 0.0f);
    matrix = ((FrameHead *)obj->clump->parent)->modelling;
    RwMatrixTranslate(&matrix, &obj->translation, rwCOMBINEPOSTCONCAT);
    rwp_object_set_matrix(obj->rwp_object, &matrix);
    rwp_object_set_active(obj->rwp_object, 0);
  }
}

/* spline_objects_destroy: releases every spline object (called from the course teardown) */
void spline_objects_destroy(void)
{
  int i;

  for (i = 0; i < SPLINE_OBJECT_COUNT; i++) {
    spline_object_destroy(g_course_spline_8c5500[i]);
  }
}

