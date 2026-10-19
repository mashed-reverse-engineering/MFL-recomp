// cluster-entries: weapon_mine_drop
/* Installed cluster weapon_mine_drop (tools/cluster_install.py from output/cluster_pilot8/weapon_mine_drop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467e90*/ extern int course_get_rpworld_00467e90();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00467fd0*/ extern int __cdecl course_surface_id_is_active_check(undefined4 p0);
/*004c4720*/ extern void FUN_004c4720(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005de25c*/ extern float k_f_0_005;
/*004726a0*/ extern int sfx_play_at(int sound, float *position);
/* a dropped mine slot (the pool in the weapon state): matrix of the placed mine, position, clump */
typedef struct MineSlot {
    RwMatrix matrix;       /* 0x00 */
    float position[3];     /* 0x40 */
    float radius;          /* 0x4c */
    char unknown_50[8];
    RpClump *clump;        /* 0x58 */
    float timer_5c;
    int state;             /* 0x60: 2 = placed */
    float timer_64;
} MineSlot;

/* weapon_mine_drop */

/* WARNING: Unknown calling convention */
/* "places a mine at the car (matrix rotate / transform) and adds its clump to the course world" */

void weapon_mine_drop_n(int in_eax)

{
  MineSlot *pRVar1;
  undefined4 uVar2;
  int iVar3;
  RpWorld *world;
  RpClump *clump;
  float *pfVar6;
  float *pfVar7;
  struct {
    float local_b4;
    float local_b0;
    float local_ac;
    float local_a8;
    float local_a4;
    float local_a0;
    float local_9c;
    float local_98;
    float local_94;
    RwV3d local_90;
    undefined4 local_84;
    float local_80;
    float local_7c;
    float local_78;
    int local_74;
    undefined1 local_70 [36];
    int local_4c;
    undefined4 local_48;
    int local_44;
    RwMatrix local_40;
  } frame;
  
  iVar3 = *(int *)(in_eax + 0xc);
  frame.local_a8 = *(float *)(iVar3 + 0x30);
  frame.local_90.z = *(float *)(iVar3 + 0x18) * k_f_n1;
  frame.local_a4 = *(float *)(iVar3 + 0x34);
  frame.local_a0 = *(float *)(iVar3 + 0x38);
  frame.local_9c = *(float *)(iVar3 + 0x10) * k_f_n1 * k_f_0_5 + frame.local_a8;
  pfVar7 = &frame.local_80;
  pfVar6 = &frame.local_a8;
  frame.local_98 = *(float *)(iVar3 + 0x14) * k_f_n1 * k_f_0_5 + frame.local_a4;
  frame.local_94 = frame.local_90.z * k_f_0_5 + frame.local_a0;
  uVar2 = weapon_get_rw_801b14();
  iVar3 = world_intersect_line(uVar2,pfVar6,pfVar7);
  if (iVar3 != 0) {
    frame.local_90.x = 1.0;
    frame.local_90.y = 0.0;
    frame.local_90.z = 0.0;
    frame.local_84 = frame.local_48;
    iVar3 = course_surface_id_is_active_check
                      (*(undefined4 *)
                        (*(int *)(frame.local_44 + 0x10) +
                        ((uint)*(ushort *)(*(int *)(frame.local_4c + 4) + 6 + frame.local_74 * 8) +
                        (uint)*(ushort *)(frame.local_4c + 0x80)) * 4));
    if (iVar3 == 0) {
      FUN_004c4720(&frame.local_b4,&frame.local_a8,&frame.local_9c,frame.local_84);
      frame.local_80 = frame.local_80 * k_f_0_005;
      iVar3 = *(int *)(in_eax + 8) + -1;
      *(int *)(in_eax + 8) = iVar3;
      frame.local_b4 = frame.local_80 + frame.local_b4;
      frame.local_b0 = frame.local_7c * k_f_0_005 + frame.local_b0;
      frame.local_ac = frame.local_78 * k_f_0_005 + frame.local_ac;
      pRVar1 = *(MineSlot **)(in_eax + iVar3 * 4);
      *(undefined4 *)(in_eax + iVar3 * 4) = 0;
      pRVar1->timer_5c = 0.0;
      pRVar1->state = 2;
      pRVar1->timer_64 = 0.0;
      pRVar1->position[0] = frame.local_b4;
      pRVar1->position[1] = frame.local_b0;
      pRVar1->position[2] = frame.local_ac;
      pRVar1->radius = 0.125;
      FUN_004c51a0(&frame.local_40,frame.local_70,&frame.local_b4);
      pRVar1->matrix = frame.local_40;
      RwMatrixRotate(&frame.local_40,&frame.local_90,-90.0,rwCOMBINEPRECONCAT);
      RwFrameTransform(*(RwFrame **)((char *)pRVar1->clump + 4),&frame.local_40,rwCOMBINEREPLACE);
      clump = pRVar1->clump;
      world = (RpWorld *)course_get_rpworld_00467e90();
      RpWorldAddClump(world,clump);
    }
  }
  return;
}

/* entry thunk at the original address of weapon_mine_drop: register inputs (eax) -> natural C weapon_mine_drop_n; ret 0 */
__declspec(naked) void weapon_mine_drop(void)
{
    __asm {
        push eax
        call weapon_mine_drop_n
        add esp, 4
        ret
    }
}
