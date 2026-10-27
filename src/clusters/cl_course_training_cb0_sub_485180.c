// cluster-entries: course_training_cb0_sub_485180
/* Installed cluster course_training_cb0_sub_485180 (tools/cluster_install.py from output/cluster_pilot_kc/course_training_cb0_sub_485180.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*005501d0*/ extern struct RpMaterial * RpMatFXMaterialSetEffects(struct RpMaterial *p0, enum RpMatFXMaterialFlags p1);
/*005504c0*/ extern undefined __cdecl RpMatFXMaterialGetEffects(undefined4 p0);
/* course_training_cb0_sub_485180 */

/* single caller course_training_cb0 */

/* (inferred names) removes the MatFX effect (type 4) from every material of the atomics of clump; returns 0 when the clump has more than 0x100 atomics, else 1 */
undefined4 course_training_cb0_sub_485180(undefined4 clump)

{
  RpMaterial *material;
  uint atomic_count;
  int atomics_found;
  int geometry;
  int effects;
  int atomic_index;
  int material_index;
  int atomics [256];
  
  atomic_count = clump_get_atomics(clump,0);
  if (atomic_count < 0x101) {
    atomics_found = clump_get_atomics(clump,atomics);
    atomic_index = 0;
    if (0 < atomics_found) {
      do {
        geometry = func_0x005501c0(atomics[atomic_index]);
        if (geometry != 0) {
          geometry = *(int *)(atomics[atomic_index] + 0x18);
          material_index = 0;
          if (0 < *(int *)(geometry + 0x24)) {
            do {
              material = *(RpMaterial **)(*(int *)(geometry + 0x20) + material_index * 4);
              effects = RpMatFXMaterialGetEffects(material);
              if (effects == 4) {
                RpMatFXMaterialSetEffects(material,rpMATFXEFFECTNULL);
              }
              material_index = material_index + 1;
            } while (material_index < *(int *)(geometry + 0x24));
          }
        }
        atomic_index = atomic_index + 1;
      } while (atomic_index < atomics_found);
    }
    return 1;
  }
  return 0;
}

