#include "mfl_tier2.h"
/*00421e20*/ extern int vehicle_set_active();
/*0047a100*/ extern int car_reset_state();
/*0047b480*/ extern double car_probe_ground_distance();
/*004904a0*/ extern undefined __cdecl car_set_physics_matrix(int p0, RwMatrix *p1, undefined4 p2, void *p3, void *p4, undefined4 p5, undefined4 p6, void *p7, void *p8, undefined4 p9, undefined4 p10, void *p11, void *p12, undefined4 p13, void *p14, void *p15, undefined4 p16);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0062ac74*/ extern float g_car_62ac74;
/*008b06e0*/ extern struct Car player_car[16];
static int car_wheel_contact_update_r(int reg_eax, int reg_edi, int stk0);
extern void car_wheel_contact_update(void);
__declspec(naked) int car_wheel_contact_update_r(int reg_eax, int reg_edi, int stk0)
{
    __asm {
        push edi
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call car_wheel_contact_update
        add esp, 4
        pop edi
        ret
    }
}

/* car_spawn */

/* "places a car: (car, x, y, z, heading): both matrices translated and rotated, contacts updated,
   car_reset_state" */

undefined4
car_spawn(uint car_index,RwReal x,float y,RwReal z,RwReal angle,undefined4 probe_arg)

{
  float *mtx_a;
  float fVar1;
  ContactPoint *pCVar2;
  int *piVar3;
  int iVar4;
  double fVar5;
  undefined4 uVar6;
  RwMatrix *mtx_b;
  undefined *puVar7;
  undefined4 uVar8;
  float *pfVar9;
  undefined *puVar10;
  RwReal RVar11;
  undefined4 uVar12;
  RwMatrix *pRVar13;
  RwV3d *translation;
  undefined4 uVar14;
  float *pfVar15;
  RwV3d *translation_00;
  undefined4 uVar16;
  RwV3d local_c;
  
  if (0xf < car_index) {
    return 0;
  }
  player_car[car_index].mode = 0;
  player_car[car_index].mode2 = 0;
  fVar5 = (double)car_probe_ground_distance(*(int *)&x,*(int *)&y,*(int *)&z,probe_arg);
  pCVar2 = player_car[car_index].contacts;
  iVar4 = 0x12;
  do {
    pCVar2->triangle = -1;
    pCVar2 = pCVar2 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  piVar3 = &player_car[car_index].wheels[0].contact_triangle;
  iVar4 = 4;
  do {
    *piVar3 = -1;
    piVar3 = piVar3 + 0x31;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  player_car[car_index].matrix_current = 0;
  player_car[car_index].matrix_previous = 1;
  player_car[car_index].velocity_history_index = 0;
  fVar1 = player_car[car_index].matrices[3];
  mtx_a = player_car[car_index].matrices;
  player_car[car_index].matrices[4] = 0.0;
  player_car[car_index].matrices[2] = 0.0;
  player_car[car_index].matrices[1] = 0.0;
  player_car[car_index].matrices[9] = 0.0;
  player_car[car_index].matrices[8] = 0.0;
  player_car[car_index].matrices[6] = 0.0;
  player_car[car_index].matrices[0xe] = 0.0;
  player_car[car_index].matrices[0xd] = 0.0;
  player_car[car_index].matrices[0xc] = 0.0;
  mtx_b = (RwMatrix *)(player_car[car_index].matrices + 0x10);
  player_car[car_index].matrices[10] = 1.0;
  player_car[car_index].matrices[5] = 1.0;
  *mtx_a = 1.0;
  *(uint *)&player_car[car_index].matrices[3] = *(uint *)&fVar1 | 0x20003;
  *(uint *)&player_car[car_index].matrices[0x13] = *(uint *)&player_car[car_index].matrices[0x13] | 0x20003;
  player_car[car_index].matrices[0x1a] = 1.0;
  player_car[car_index].matrices[0x15] = 1.0;
  (mtx_b->right).x = 1.0;
  uVar16 = 0;
  translation_00 = &local_c;
  player_car[car_index].matrices[0x14] = 0.0;
  player_car[car_index].matrices[0x12] = 0.0;
  player_car[car_index].matrices[0x11] = 0.0;
  player_car[car_index].matrices[0x19] = 0.0;
  player_car[car_index].matrices[0x18] = 0.0;
  player_car[car_index].matrices[0x16] = 0.0;
  player_car[car_index].matrices[0x1e] = 0.0;
  player_car[car_index].matrices[0x1d] = 0.0;
  player_car[car_index].matrices[0x1c] = 0.0;
  local_c.y = (RwReal)((double)y - fVar5);
  local_c.x = x;
  local_c.z = z;
  pfVar15 = mtx_a;
  RwMatrixTranslate((RwMatrix *)mtx_a,translation_00,rwCOMBINEREPLACE);
  uVar14 = 0;
  translation = &local_c;
  pRVar13 = mtx_b;
  RwMatrixTranslate(mtx_b,translation,rwCOMBINEREPLACE);
  uVar12 = 1;
  puVar10 = (undefined *)&g_car_62ac74;
  pfVar9 = mtx_a;
  RVar11 = angle;
  RwMatrixRotate((RwMatrix *)mtx_a,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  uVar8 = 1;
  puVar7 = (undefined *)&g_car_62ac74;
  RwMatrixRotate(mtx_b,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  uVar6 = 0;
  car_wheel_contact_update_r(probe_arg,(int)(player_car + car_index),0);
  car_set_physics_matrix
            (car_index,(RwMatrix *)mtx_a,uVar6,mtx_b,puVar7,*(int *)&angle,uVar8,pfVar9,puVar10,*(int *)&RVar11,
             uVar12,pRVar13,translation,uVar14,pfVar15,translation_00,uVar16);
  vehicle_set_active(car_index,1);
  car_reset_state(player_car[car_index].index);
  return 1;
}


