// cluster-entries: weapon_effect_scale_by_camera_distance
/* Installed cluster weapon_effect_scale_by_camera_distance (tools/cluster_install.py from output/cluster_pilot10/weapon_effect_scale_by_camera_distance.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();
/*00484770*/ extern double distance_between_points();
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd32c*/ extern float k_f_3;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005de258*/ extern float k_f_0_003;
/* weapon_effect_scale_by_camera_distance */

/* Scales the effect's frame: x,y = 4 * clamp(distance to camera, 0.003, 3) * 0.5, z = effect[0x1c] * 4 (inferred) */

void weapon_effect_scale_by_camera_distance_n(int effect)

{
  int camera;
  RwMatrix *matrix;
  float10 distance;
  RwV3d scale;
  
  scale.z = *(float *)(effect + 0x1c) * k_f_4;
  scale.x = 4.0;
  scale.y = 4.0;
  camera = get_viewport_camera(0);
  distance = (float10)distance_between_points
                             (effect + 0x48,effect + 0x3c,*(int *)(camera + 4) + 0x40);
  if (!((float10)k_f_0_003 > distance)) {
    if ((float10)k_f_3 < distance) {
      distance = (float10)k_f_3;
    }
  }
  else {
    distance = (float10)k_f_0_003;
  }
  scale.x = (RwReal)((float10)scale.x * distance * (float10)k_f_0_5);
  scale.y = (RwReal)(distance * (float10)k_f_0_5 * (float10)scale.y);
  matrix = (RwMatrix *)(*(char **)(*(int *)(effect + 0x54) + 4) + 0x10);
  RwMatrixScale(matrix,&scale,rwCOMBINEPRECONCAT);
  RwMatrixUpdate(matrix);
  RwFrameUpdateObjects(*(RwFrame **)(*(char **)(effect + 0x54) + 4));
  return;
}

/* entry thunk at the original address of weapon_effect_scale_by_camera_distance: register inputs (edi) -> natural C weapon_effect_scale_by_camera_distance_n; ret 0 */
__declspec(naked) void weapon_effect_scale_by_camera_distance(void)
{
    __asm {
        push edi
        call weapon_effect_scale_by_camera_distance_n
        add esp, 4
        ret
    }
}
