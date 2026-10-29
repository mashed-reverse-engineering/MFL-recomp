#include "mfl_tier2.h"

/* Course clump offsets within CourseWorld (clumps array starts at offset 0xc110) */
#define COURSE_WORLD_CLUMP_1_OFFSET 0xc114
#define COURSE_WORLD_CLUMP_2_OFFSET 0xc118
#define COURSE_WORLD_CLUMP_3_OFFSET 0xc11c
#define COURSE_WORLD_CLUMP_5_OFFSET 0xc124

/* Unknown parameter for stub function (0x40100000 = 2.25f in IEEE 754) */
#define UNKNOWN_STUB_PARAM_40100000 0x40100000

/* RenderWare Z-write enable states */
#define RW_ZWRITE_DISABLE 0x0
#define RW_ZWRITE_ENABLE  0x1

/*00405e40*/ extern int stub_return_zero();
/*0044a010*/ extern int course_sky_atomic_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00405e40*/ extern int stub_return_zero();
/*0044a010*/ extern int course_sky_atomic_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* Render callback (cb2) for course "KEYS": renders course model clumps and sky. */
void course_keys_cb2(char *course_world)
{
  /* Render course clump 1 if present */
  if (*(RpClump **)(course_world + COURSE_WORLD_CLUMP_1_OFFSET) != (RpClump *)NULL) {
    RpClumpRender(*(RpClump **)(course_world + COURSE_WORLD_CLUMP_1_OFFSET));
  }
  /* Render course clump 2 if present */
  if (*(RpClump **)(course_world + COURSE_WORLD_CLUMP_2_OFFSET) != (RpClump *)NULL) {
    RpClumpRender(*(RpClump **)(course_world + COURSE_WORLD_CLUMP_2_OFFSET));
  }
  /* unknown: stubbed call with unknown parameter (possibly float 2.25f) */
  stub_return_zero(0,UNKNOWN_STUB_PARAM_40100000);
  /* Render course clump 5 if present */
  if (*(RpClump **)(course_world + COURSE_WORLD_CLUMP_5_OFFSET) != (RpClump *)NULL) {
    RpClumpRender(*(RpClump **)(course_world + COURSE_WORLD_CLUMP_5_OFFSET));
  }
  /* unknown: stubbed call */
  stub_return_zero(0,0);
  /* Render course clump 3 if present */
  if (*(RpClump **)(course_world + COURSE_WORLD_CLUMP_3_OFFSET) != (RpClump *)NULL) {
    RpClumpRender(*(RpClump **)(course_world + COURSE_WORLD_CLUMP_3_OFFSET));
  }
  /* Render sky atomic with Z-write disabled so background does not occlude geometry */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RW_ZWRITE_DISABLE);
  course_sky_atomic_render();
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RW_ZWRITE_ENABLE);
  return;
}
