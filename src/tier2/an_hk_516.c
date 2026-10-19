#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*00453fb0*/ extern int course_shadow_particles_render();
/*004a5d70*/ extern int object_render();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*005df5d4*/ extern float k_f_n0_01;
/*006a267c*/ extern undefined4 g_course_highway_6a267c;
/*006a2690*/ extern undefined4 g_course_highway_6a2690;
/*006a2694*/ extern undefined4 g_course_highway_6a2694;
/*006a2698*/ extern undefined4 g_course_highway_6a2698;
/*006a269c*/ extern undefined4 g_course_highway_6a269c;
/*006a26a0*/ extern undefined4 g_course_highway_6a26a0;
/*006a26b4*/ extern undefined4 g_course_highway_6a26b4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf140*/ extern undefined4 g_course_8bf140;
/*008c4000*/ extern char g_course_set_8c4000[];
/* Per-frame draw pass for the highway course (course_highway_cb2).
 * Walks the course entry table (g_course_8bf140 up to g_course_set_8c4000, ENTRY_STRIDE ints each):
 * each entry is frustum-tested against the current camera, its visible flag is stored at
 * ENTRY_VISIBLE_IDX, and if visible its virtual slot +0x48 is called (inferred: update/render hook),
 * plus its sub-objects when the status is 0. Then renders the highway clump at
 * course_data + HIGHWAY_CLUMP_OFFSET, the shadow particles on COURSE_HIGHWAYN, and the five fixed objects.
 * course_data: course state pointer (inferred). Returns nothing. */
#define COURSE_HIGHWAYN (12)
#define HIGHWAY_CLUMP_OFFSET (0xc1bc) /* inferred; an_hk_535 uses 0xc1d8 */
#define ENTRY_STRIDE (0x3f)
#define ENTRY_VISIBLE_IDX (0x16)
#define ENTRY_SUBOBJ_COUNT_IDX (-0xf)
#define SUB_ENTRY_STRIDE (6)
#define RGBA_WHITE (0xffffffff)
#define RS_ZWRITE_OFF ((void *)0x0)
#define RS_ZWRITE_ON ((void *)0x1)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_highway_cb2(char *course_data)

{
  RwCamera *camera;
  undefined4 brush;
  RwFrustumTestResult frustum_result;
  int status;
  CourseId current_course;
  int *sub_entry;
  int *entry;
  undefined4 rgba;

  rgba = RGBA_WHITE;
  camera = RwCameraGetCurrentCamera();
  func_0x0042b900();
  brush = func_0x0042b910();
  g_course_highway_6a267c = camera;
  Rt2dBrushSetRGBA(brush,&rgba,&rgba,&rgba,&rgba);
  entry = &g_course_8bf140;
  /* loop over course entries until the end of the table at g_course_set_8c4000 */
  do {
    frustum_result = RwCameraFrustumTestSphere(g_course_highway_6a267c,(RwSphere *)(entry + -8));
    entry[ENTRY_VISIBLE_IDX] = (uint)(frustum_result != rwSPHEREOUTSIDE);
    if ((frustum_result != rwSPHEREOUTSIDE) != 0) {
      status = func_0x005685e0(*entry);
      (**(code **)(*entry + 0x48))(*entry);
      /* visible entry with sub-objects: call the hook on each sub-object (inferred) */
      if ((status == 0) && (0 < entry[ENTRY_SUBOBJ_COUNT_IDX])) {
        sub_entry = entry + -0x25;
        status = 0;
        do {
          (**(code **)(*sub_entry + 0x48))(*sub_entry);
          status = status + 1;
          sub_entry = sub_entry + SUB_ENTRY_STRIDE;
        } while (status < entry[ENTRY_SUBOBJ_COUNT_IDX]);
      }
      if (*(float *)&entry[0xd] < k_f_n0_01) {
        (**(code **)(entry[1] + 0x48))(entry[1]);
      }
      status = entry[2];
      if (status != 0) {
        (**(code **)(status + 0x48))(status);
      }
    }
    entry = entry + ENTRY_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_course_set_8c4000));
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,RS_ZWRITE_OFF);
  if (g_course_highway_6a26b4 != 0) {
    RpClumpRender(*(RpClump **)(course_data + HIGHWAY_CLUMP_OFFSET));
  }
  func_0x004541a0();
  /* shadow particles only on the highway course */
  current_course = current_course_id();
  if (current_course == COURSE_HIGHWAYN) {
    course_shadow_particles_render();
  }
  func_0x00453c00();
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,RS_ZWRITE_ON);
  object_render(g_course_highway_6a2690);
  object_render(g_course_highway_6a2694);
  object_render(g_course_highway_6a2698);
  object_render(g_course_highway_6a269c);
  object_render(g_course_highway_6a26a0);
  return;
}


