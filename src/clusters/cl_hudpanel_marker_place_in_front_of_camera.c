// cluster-entries: hudpanel_marker_place_in_front_of_camera
/* Installed cluster hudpanel_marker_place_in_front_of_camera (tools/cluster_install.py from output/cluster_pilot9/hudpanel_marker_place_in_front_of_camera.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00422320*/ extern int vehicle_get_matrix_local();
/*00472890*/ extern int sfx_play_voice_at();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd418*/ extern float k_f_12;
/*005de274*/ extern float k_f_57_295776;
/*005de278*/ extern float k_f_n12;
extern double atan2(double, double);
/* hudpanel_marker_place_in_front_of_camera */

/* WARNING: Unknown calling convention */
/* "places the marker frame, offsets by 12 along the horizontal direction and plays voice 0x12/0x14"
    */

void hudpanel_marker_place_in_front_of_camera_n(undefined4 vehicle, int marker)

{
  float *pos_ptr;
  char *frame_ptr;
  float10 heading;
  struct {
    RwV3d facing_flat;
    RwV3d right_flat;
    RwMatrix vehicle_matrix;
  } frame;
  
  *(undefined4 *)(marker + 0x44) = 1;
  *(undefined4 *)(marker + 0x48) = 0;
  *(undefined4 *)(marker + 0x4c) = vehicle;
  vehicle_get_matrix_local(vehicle,&frame.vehicle_matrix);
  RwFrameTransform(*(RwFrame **)(*(char **)(marker + 0x40) + 4),&frame.vehicle_matrix,rwCOMBINEREPLACE);
  frame_ptr = *(char **)(*(char **)(marker + 0x40) + 4);
  pos_ptr = (float *)(frame_ptr + 0x40);
  frame.right_flat.x = *(RwReal *)(frame_ptr + 0x10);
  frame.right_flat.z = *(RwReal *)(frame_ptr + 0x18);
  frame.right_flat.y = 0.0;
  RwV3dNormalize(&frame.right_flat,&frame.right_flat);
  frame.facing_flat.x = *(float *)(frame_ptr + 0x30);
  frame.facing_flat.z = *(float *)(frame_ptr + 0x38);
  frame.facing_flat.y = 0.0;
  RwV3dNormalize(&frame.facing_flat,&frame.facing_flat);
  *pos_ptr = frame.facing_flat.x * k_f_n12 + *pos_ptr;
  *(float *)(frame_ptr + 0x44) = frame.facing_flat.y * k_f_n12 + *(float *)(frame_ptr + 0x44);
  *pos_ptr = *pos_ptr;
  *(float *)(frame_ptr + 0x48) = frame.facing_flat.z * k_f_n12 + *(float *)(frame_ptr + 0x48);
  *(undefined4 *)(frame_ptr + 0x48) = *(undefined4 *)(frame_ptr + 0x48);
  *(float *)(frame_ptr + 0x44) = *(float *)(frame_ptr + 0x44) + k_f_12;
  RwMatrixUpdate(*(int *)(*(char **)(marker + 0x40) + 4) + 0x10);
  heading = (float10)atan2((float10)frame.facing_flat.x,(float10)frame.facing_flat.z);
  *(float *)(marker + 0x58) = (float)(heading * (float10)k_f_57_295776);
  *(undefined4 *)(marker + 0x5c) = 0x42340000;
  sfx_play_voice_at(0x12,pos_ptr,*(undefined4 *)(marker + 0x60));
  sfx_play_voice_at(0x14,pos_ptr,*(undefined4 *)(marker + 0x60));
  return;
}

/* entry thunk at the original address of hudpanel_marker_place_in_front_of_camera: register inputs (eax,edi) -> natural C hudpanel_marker_place_in_front_of_camera_n; ret 0 */
__declspec(naked) void hudpanel_marker_place_in_front_of_camera(void)
{
    __asm {
        push edi
        push eax
        call hudpanel_marker_place_in_front_of_camera_n
        add esp, 8
        ret
    }
}
