// cluster-entries: course_lod_atomic_set_geometries
/* Installed cluster course_lod_atomic_set_geometries (tools/cluster_install.py from output/cluster_pilot_uc3/course_lod_atomic_set_geometries.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00568490*/ extern void RpLODAtomicSetGeometry(int atomic, int index, int geometry);
/*00568600*/ extern void RpLODAtomicSetRange(int atomic, float range);

/* Sets the 10 LOD geometries of `atomic`: level 0 from its own geometry (+0x18), level 1 from `second` (+0x18), levels 2..9 from `lower` (+0x18); LOD range 200.0. */
void course_lod_atomic_set_geometries_n(int atomic, int lower, int second)
{
  int level;

  RpLODAtomicSetGeometry(atomic, 0, *(int *)(atomic + 0x18));
  RpLODAtomicSetGeometry(atomic, 1, *(int *)(second + 0x18));
  for (level = 2; level < 10; level++) {
    RpLODAtomicSetGeometry(atomic, level, *(int *)(lower + 0x18));
  }
  RpLODAtomicSetRange(atomic, 200.0f);
}

/* entry thunk at the original address of course_lod_atomic_set_geometries: register inputs (esi,edi,s0) -> natural C course_lod_atomic_set_geometries_n; ret 0 */
__declspec(naked) void course_lod_atomic_set_geometries(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edi
        push esi
        call course_lod_atomic_set_geometries_n
        add esp, 12
        ret
    }
}
