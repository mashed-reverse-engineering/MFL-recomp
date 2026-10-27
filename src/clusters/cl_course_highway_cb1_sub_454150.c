// cluster-entries: course_highway_cb1_sub_454150
/* Installed cluster course_highway_cb1_sub_454150 (tools/cluster_install.py from output/cluster_pilot_md/course_highway_cb1_sub_454150.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005685e0*/ extern int __cdecl SUB_005685e0(char *node);
/*00486c40*/ extern int particles_flush(int pool);
/*006a1860*/ extern int g_course_car_6a1860;
/* course_highway_cb1_sub_454150 */

#define HIGHWAY_OBJECT_COUNT 80
#define HIGHWAY_OBJECT_BASE_BACK 0xa0   /* the object proper starts this many bytes before the g_highway_objects entry */

/* one highway object (0xfc bytes): the scene node it follows and a visible flag; SUB_00454020 gets the object itself (0xa0 bytes before the node field) */
typedef struct HighwayObject {
  char *node;                 /* +0x00 */
  char _gap_4[0x4c];
  int visible;                /* +0x50 */
  char _gap_54[0xa8];
} HighwayObject;

extern HighwayObject g_highway_objects[HIGHWAY_OBJECT_COUNT];   /* 0x8bf140 */

static void SUB_00454020_r(char * p0);
extern void SUB_00454020(void);
/* "single caller course_highway_cb1: updates every visible highway object that is close to the scene, then flushes the particles of the car" */
void course_highway_cb1_sub_454150(void)
{
  HighwayObject *object;

  for (object = g_highway_objects; object < g_highway_objects + HIGHWAY_OBJECT_COUNT; object++) {
    if (object->visible != 0 && SUB_005685e0(object->node) < 2) {
      SUB_00454020_r((char *)object - HIGHWAY_OBJECT_BASE_BACK);
    }
  }
  particles_flush(g_course_car_6a1860);
}

/* SUB_00454020_r: C -> original SUB_00454020 (registers eax) */
__declspec(naked) void SUB_00454020_r(char * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call SUB_00454020
        ret
    }
}
