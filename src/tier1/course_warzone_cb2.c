#include "mfl_tier2.h"
/*0045c7f0*/ extern int course_atomics_render_3();
/*0045d560*/ extern int course_clump_render_with_physics();
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a2888*/ extern undefined g_course_6a2888;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*008bf080*/ extern char g_course_8bf080[];

#define RW_CULL_NONE 0x1 /* rwCULLMODECULLNONE */
#define RW_CULL_BACK 0x2 /* rwCULLMODECULLBACK */
#define WARZONE_OBJECT_LIST_END ((int)&g_course_8bf080) /* end address of g_course_warzone_8bf060 list */


/* course_warzone_cb2 */

/* Warzone course render callback: draws course parts with culling off, then back-face culling for the rest. */
void course_warzone_cb2(char *course)
{
  undefined4 *object_slot;
  
  course_atomics_render_3();
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)RW_CULL_NONE);
  /* course+0xc15c and course+0xc160: optional clumps (names unknown) */
  if (*(RpClump **)(course + 0xc15c) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc15c));
  }
  if (*(RpClump **)(course + 0xc160) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc160));
  }
  /* g_course_warzone_6a29a4: object passed to object_render (purpose unknown) */
  object_render(g_course_warzone_6a29a4);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)RW_CULL_BACK);
  /* g_course_6a2888: course clump, rendered with physics (purpose unknown) */
  course_clump_render_with_physics(&g_course_6a2888);
  /* course+0xc110: clump (name unknown) */
  RpClumpRender(*(RpClump **)(course + 0xc110));
  /* g_course_warzone_8bf060 = list of object pointers; each goes to func_0x0045d9f0 (name unknown) */
  object_slot = &g_course_warzone_8bf060;
  do {
    func_0x0045d9f0(*object_slot);
    object_slot = object_slot + 1;
  } while ((int)object_slot < WARZONE_OBJECT_LIST_END);
  /* course+0xc120 and course+0xc124: optional clumps (names unknown) */
  if (*(RpClump **)(course + 0xc120) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc120));
  }
  if (*(RpClump **)(course + 0xc124) != (RpClump *)0x0) {
    RpClumpRender(*(RpClump **)(course + 0xc124));
  }
  return;
}
