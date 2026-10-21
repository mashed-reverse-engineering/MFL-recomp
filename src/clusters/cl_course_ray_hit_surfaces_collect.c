// cluster-entries: course_ray_hit_surfaces_collect
/* Installed cluster course_ray_hit_surfaces_collect (tools/cluster_install.py from output/cluster_pilot_uc3/course_ray_hit_surfaces_collect.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(float *hit, float *position);
/*004c4ba0*/ extern int __cdecl FUN_004c4ba0(int world, float *ray, void *hits, int max_hits);

/* One ray intersection record filled by FUN_004c4ba0 (64 bytes). */
typedef struct RayHit {
  char head[0xc];
  int triangle;        /* triangle index inside the sector */
  char body[0x24];
  char *sector;        /* +0x34: +4 triangle table (8 bytes per triangle, material index at +6), +0x80 material base */
  float distance;
  char *object;        /* +0x3c: +0x10 material list */
} RayHit;

/* Casts `ray` into `world` (up to 3 hits), keeps the hits for which rocket_surface_hit_test says 0 (walking the records from the last to the first):
   their distances go to out_distance[]; with out_ids the packed colour of the hit material is registered as a course object id (course_object_id_register)
   and its index stored at the same position of out_ids[]. Returns the number of kept hits. */
static int course_object_id_register_r(int p0);
extern void course_object_id_register(void);
int course_ray_hit_surfaces_collect_n(int world, float *ray, float *out_distance, int *out_ids)
{
  RayHit hits[3] = {0};
  int count;
  int kept = 0;
  int i;
  int material;

  count = FUN_004c4ba0(world, ray, hits, 3);
  for (i = count; i > 0; i--) {
    RayHit *hit = &hits[i - 1];
    if (rocket_surface_hit_test((float *)hit, ray) != 0) {
      continue;
    }
    if (out_ids != 0) {
      material = *(unsigned short *)(*(char **)(hit->sector + 4) + hit->triangle * 8 + 6) + *(unsigned short *)(hit->sector + 0x80);
      out_ids[kept] = course_object_id_register_r(*(int *)(*(char **)(*(char **)(hit->object + 0x10) + material * 4) + 4));
    }
    *out_distance++ = hit->distance;
    kept++;
  }
  return kept;
}

/* course_object_id_register_r: C -> original course_object_id_register (registers edx) */
__declspec(naked) int course_object_id_register_r(int p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call course_object_id_register
        ret
    }
}

/* entry thunk at the original address of course_ray_hit_surfaces_collect: register inputs (edx,s0,s1,s2) -> natural C course_ray_hit_surfaces_collect_n; ret 0 */
__declspec(naked) void course_ray_hit_surfaces_collect(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push edx
        call course_ray_hit_surfaces_collect_n
        add esp, 16
        ret
    }
}
