// cluster-entries: course_highway_lod_atomic_clone_a
/* Installed cluster course_highway_lod_atomic_clone_a (tools/cluster_install.py from output/cluster_pilot_hw/course_highway_lod_atomic_clone_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f6ae0*/ extern struct RpAtomic * RpAtomicClone(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/* 00459350: the original takes THREE inputs: ESI = destination atomic, EDI = far LOD atomic, and ONE STACK ARGUMENT (the near LOD atomic, the caller pops it with add esp);
   the first pilot passed only (esi, edi), so the original read an uninitialised stack slot as the near LOD atomic -> RpLODAtomicSetGeometry(.., 1, garbage geometry) */
/* course_highway_lod_atomic_clone_a */

/* "clones the atomic, creates a frame, sets LOD geometries" */

static void course_lod_atomic_set_geometries_r(RpAtomic * p0, RpAtomic * p1, RpAtomic * p2);
extern void course_lod_atomic_set_geometries(void);
/* lods[0] = the atomic to clone, lods[1] = near LOD atomic, lods[2] = far LOD atomic; object = the highway LOD object (+0xA0 = the clone) */
void course_highway_lod_atomic_clone_a_n(RpAtomic **lods, char *object)

{
  RpAtomic *clone;
  RwFrame *frame;

  clone = RpAtomicClone(lods[0]);
  *(RpAtomic **)(object + 0xa0) = clone;
  frame = RwFrameCreate();
  RpAtomicSetFrame(*(RpAtomic **)(object + 0xa0),frame);
  course_lod_atomic_set_geometries_r(*(RpAtomic **)(object + 0xa0),lods[2],lods[1]);
  return;
}

/* course_lod_atomic_set_geometries_r: C -> original course_lod_atomic_set_geometries (registers esi,edi,s0) */
__declspec(naked) void course_lod_atomic_set_geometries_r(RpAtomic * p0, RpAtomic * p1, RpAtomic * p2)
{
    __asm {
        push esi
        push edi
        push dword ptr [esp + 20]
        mov esi, dword ptr [esp + 16]
        mov edi, dword ptr [esp + 20]
        call course_lod_atomic_set_geometries
        add esp, 4
        pop edi
        pop esi
        ret
    }
}

/* entry thunk at the original address of course_highway_lod_atomic_clone_a: register inputs (eax,ebx) -> natural C course_highway_lod_atomic_clone_a_n; ret 0 */
__declspec(naked) void course_highway_lod_atomic_clone_a(void)
{
    __asm {
        push ebx
        push eax
        call course_highway_lod_atomic_clone_a_n
        add esp, 8
        ret
    }
}
