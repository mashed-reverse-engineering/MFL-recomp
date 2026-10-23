// cluster-entries: hudpanel_marker_place
/* Installed cluster hudpanel_marker_place (tools/cluster_install.py from output/cluster_pilot_uc3/hudpanel_marker_place.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, undefined4 p1);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005ddad4*/ extern float k_f_90;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005de26c*/ extern float k_f_1_2;
/*005de284*/ extern float k_f_1_1111112;
/*00603ea4*/ extern undefined4 g_marker_particles_603ea4;
/* hudpanel_marker_place */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "builds the matrices of a player marker (translate / scale / rotate) and applies them to its
   frame" */

static float marker_x_f32;
void hudpanel_marker_place_n(int marker)

{
  int part;
  float spread;
  uint flags;
  uint visible;
  float *rec_ptr;
  float *px;
  int rec_or_part;
  int *part_ptr;
  struct {
    float rot_t;
    RwV3d translate_vec;
    RwV3d scale_vec;
    RwV3d axis_vec;
    RwV3d particle_offset;
    RwMatrix mtx;
  } frame;
  
  rec_or_part = *(int *)(marker + 0x50);
  px = &frame.translate_vec.x;
  frame.translate_vec.x = 0.0;
  frame.translate_vec.y = 0.0;
  frame.translate_vec.z = 1.0;
  frame.axis_vec.x = 0.0;
  frame.axis_vec.y = 0.0;
  frame.axis_vec.z = 1.0;
  frame.scale_vec.x = 1.0;
  frame.scale_vec.y = 1.0;
  frame.scale_vec.z = 1.0;
  if (rec_or_part == 0) {
    return;
  }
  *(undefined4 *)(marker + 0x4c) = 0;
  if (*(int *)(rec_or_part + 0x34) == 2) {
    *(undefined4 *)(marker + 0x4c) = 1;
  }
  if (*(int *)(rec_or_part + 0x60) == 0) {
    if (*(int *)(rec_or_part + 0x58) == 0) goto LAB_0041c171;
    flags = *(uint *)(marker + 0x4c) | 2;
  }
  else {
    flags = *(uint *)(marker + 0x4c) | 4;
  }
  *(uint *)(marker + 0x4c) = flags;
LAB_0041c171:
  flags = *(uint *)(marker + 0x4c);
  rec_or_part = 0;
  do {
    part = *(int *)(marker + 0x20 + rec_or_part * 4);
    *(undefined4 *)(marker + rec_or_part * 4) = 0;
    if ((part != 0) && ((flags & 1) != 0)) {
      switch(rec_or_part) {
      case 0:
        visible = 1;
        break;
      case 1:
        visible = flags & 4;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
        visible = ~(flags >> 2) & 1;
        break;
      default:
        visible = 0;
      }
      *(uint *)(marker + rec_or_part * 4) = visible;
      FUN_004c5380(part,*(int *)(marker + 0x50) + 0x10);
    }
    rec_or_part = rec_or_part + 1;
  } while (rec_or_part < 8);
  rec_ptr = *(float **)(marker + 0x50);
  frame.rot_t = rec_ptr[0x17] * k_f_1_1111112;
  if ((float_1_0 <= frame.rot_t) || (k_f_0 <= frame.rot_t)) {
    if (float_1_0 <= frame.rot_t) {
      frame.rot_t = 1.0;
    }
  }
  else {
    frame.rot_t = 0.0;
  }
  frame.mtx.flags = frame.mtx.flags | 0x20003;
  frame.scale_vec.y = 1.0;
  frame.scale_vec.x = 1.0;
  frame.mtx.at.z = 1.0;
  marker_x_f32 = (((float_1_0 - *rec_ptr) + (float_1_0 - *rec_ptr)) - float_1_0) * k_f_0_5;
  frame.mtx.up.y = 1.0;
  frame.mtx.right.x = 1.0;
  frame.mtx.up.x = 0.0;
  frame.mtx.right.z = 0.0;
  frame.mtx.right.y = 0.0;
  frame.translate_vec.x = marker_x_f32 * k_f_1_2;
  frame.translate_vec.y = (((float_1_0 - rec_ptr[1]) + (float_1_0 - rec_ptr[1])) - float_1_0) * k_f_0_5 *
               k_f_0_9;
  frame.mtx.at.y = 0.0;
  frame.mtx.at.x = 0.0;
  frame.mtx.up.z = 0.0;
  frame.mtx.pos.z = 0.0;
  frame.mtx.pos.y = 0.0;
  frame.mtx.pos.x = 0.0;
  RwMatrixTranslate(&frame.mtx,&frame.translate_vec,rwCOMBINEREPLACE);
  RwMatrixScale(&frame.mtx,&frame.scale_vec,rwCOMBINEPRECONCAT);
  RwFrameTransform(*(RwFrame **)(marker + 0x44),&frame.mtx,rwCOMBINEREPLACE);
  frame.mtx.flags = frame.mtx.flags | 0x20003;
  frame.mtx.at.z = 1.0;
  frame.mtx.up.y = 1.0;
  frame.mtx.right.x = 1.0;
  frame.mtx.up.x = 0.0;
  frame.mtx.right.z = 0.0;
  frame.mtx.right.y = 0.0;
  frame.mtx.at.y = 0.0;
  frame.mtx.at.x = 0.0;
  frame.mtx.up.z = 0.0;
  frame.mtx.pos.z = 0.0;
  frame.mtx.pos.y = 0.0;
  frame.mtx.pos.x = 0.0;
  RwMatrixRotate(&frame.mtx,&frame.axis_vec,frame.rot_t * k_f_90,rwCOMBINEPRECONCAT);
  part_ptr = (int *)(marker + 0x28);
  RwFrameTransform(*(RwFrame **)(*(int *)(*(int *)(marker + 0x28) + 4) + 4),&frame.mtx,
                   rwCOMBINEREPLACE);
  rec_ptr = (float *)&g_marker_particles_603ea4;
  spread = (float_1_0 - frame.rot_t) * k_f_0_025;
  do {
    frame.particle_offset.x = rec_ptr[-1] * spread;
    frame.particle_offset.y = spread * *rec_ptr;
    frame.particle_offset.z = rec_ptr[1] * spread;
    RwFrameTranslate(*(RwFrame **)(*part_ptr + 4),&frame.particle_offset,rwCOMBINEREPLACE);
    rec_ptr = rec_ptr + 3;
    part_ptr = part_ptr + 1;
  } while ((int)rec_ptr < (int)&g_marker_particles_603ea4 + 0x25);
  return;
}

/* entry thunk at the original address of hudpanel_marker_place: register inputs (eax) -> natural C hudpanel_marker_place_n; ret 0 */
__declspec(naked) void hudpanel_marker_place(void)
{
    __asm {
        push eax
        call hudpanel_marker_place_n
        add esp, 4
        ret
    }
}
