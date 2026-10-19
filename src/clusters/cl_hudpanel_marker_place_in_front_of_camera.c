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

void hudpanel_marker_place_in_front_of_camera_n(undefined4 in_eax, int in_edi)

{
  float *pfVar1;
  char *iVar2;
  float10 fVar3;
  struct {
    RwV3d local_58;
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  *(undefined4 *)(in_edi + 0x44) = 1;
  *(undefined4 *)(in_edi + 0x48) = 0;
  *(undefined4 *)(in_edi + 0x4c) = in_eax;
  vehicle_get_matrix_local(in_eax,&frame.local_40);
  RwFrameTransform(*(RwFrame **)(*(char **)(in_edi + 0x40) + 4),&frame.local_40,rwCOMBINEREPLACE);
  iVar2 = *(char **)(*(char **)(in_edi + 0x40) + 4);
  pfVar1 = (float *)(iVar2 + 0x40);
  frame.local_4c.x = *(RwReal *)(iVar2 + 0x10);
  frame.local_4c.z = *(RwReal *)(iVar2 + 0x18);
  frame.local_4c.y = 0.0;
  RwV3dNormalize(&frame.local_4c,&frame.local_4c);
  frame.local_58.x = *(float *)(iVar2 + 0x30);
  frame.local_58.z = *(float *)(iVar2 + 0x38);
  frame.local_58.y = 0.0;
  RwV3dNormalize(&frame.local_58,&frame.local_58);
  *pfVar1 = frame.local_58.x * k_f_n12 + *pfVar1;
  *(float *)(iVar2 + 0x44) = frame.local_58.y * k_f_n12 + *(float *)(iVar2 + 0x44);
  *pfVar1 = *pfVar1;
  *(float *)(iVar2 + 0x48) = frame.local_58.z * k_f_n12 + *(float *)(iVar2 + 0x48);
  *(undefined4 *)(iVar2 + 0x48) = *(undefined4 *)(iVar2 + 0x48);
  *(float *)(iVar2 + 0x44) = *(float *)(iVar2 + 0x44) + k_f_12;
  RwMatrixUpdate(*(int *)(*(char **)(in_edi + 0x40) + 4) + 0x10);
  fVar3 = (float10)atan2((float10)frame.local_58.x,(float10)frame.local_58.z);
  *(float *)(in_edi + 0x58) = (float)(fVar3 * (float10)k_f_57_295776);
  *(undefined4 *)(in_edi + 0x5c) = 0x42340000;
  sfx_play_voice_at(0x12,pfVar1,*(undefined4 *)(in_edi + 0x60));
  sfx_play_voice_at(0x14,pfVar1,*(undefined4 *)(in_edi + 0x60));
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
