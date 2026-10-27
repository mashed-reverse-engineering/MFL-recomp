// cluster-entries: shadow_project_to_ground_00499dc0_cb_499cc0
/* Installed cluster shadow_project_to_ground_00499dc0_cb_499cc0 (tools/cluster_install.py from output/cluster_pilot_md/shadow_project_to_ground_00499dc0_cb_499cc0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004f56f0*/ extern struct RpWorld * RpWorldSectorGetWorld(struct RpWorldSector *p0);
/*005de490*/ extern float k_f_0_02;
/*005e0968*/ extern float k_f_2_5e0968;
/* a float stored and read back (the original spills these x87 values to stack slots) */
static float float_barrier(float v) { return v; }

/* shadow_project_to_ground_00499dc0_cb_499cc0 */

/* "function pointer pushed/stored by shadow_project_to_ground_00499dc0 (PUSH 0x499cc0): intersection callback, stores the hit point,
   the triangle normal pushed out by 2 %, the sector and its material in the hit record" */
undefined4 shadow_project_to_ground_00499dc0_cb_499cc0
          (float *segment, RpWorldSector *sector, float *triangle, float t, float *hit)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  RpWorld *world;
  int i;
  float *src;
  float *dst;

  hit[5] = float_barrier((segment[3] - segment[0]) * t) + segment[0];
  hit[6] = (segment[4] - segment[1]) * t + segment[1];
  hit[7] = float_barrier(segment[5] - segment[2]) * t + segment[2];
  hit[2] = triangle[0];
  hit[3] = triangle[1];
  hit[4] = triangle[2];
  fVar2 = triangle[0] * k_f_0_02;
  fVar3 = triangle[1] * k_f_0_02;
  fVar1 = triangle[2];
  *(int *)&hit[8] = 1;
  fVar1 = fVar1 * k_f_0_02;
  hit[5] = fVar2 + hit[5];
  hit[6] = fVar3 + hit[6];
  hit[7] = fVar1 + hit[7];
  hit[0x13] = k_f_2_5e0968 * t;
  src = triangle;
  dst = hit + 9;
  for (i = 10; i != 0; i = i - 1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  *(RpWorldSector **)hit = sector;
  world = RpWorldSectorGetWorld(sector);
  *(void **)&hit[1] = RpWorld_matList_materials(world)[(uint)RpWorldSector_triangles(sector)[((int *)triangle)[6]].matIndex + (uint)RpWorldSector_matListWindowBase(sector)];
  return 0;
}

