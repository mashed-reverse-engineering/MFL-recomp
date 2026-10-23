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
car_spawn(uint param_1,RwReal param_2,float param_3,RwReal param_4,RwReal angle,undefined4 param_6)

{
  float *matrix;
  float fVar1;
  ContactPoint *pCVar2;
  int *piVar3;
  int iVar4;
  double fVar5;
  undefined4 uVar6;
  RwMatrix *matrix_00;
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
  
  if (0xf < param_1) {
    return 0;
  }
  player_car[param_1].mode = 0;
  player_car[param_1].mode2 = 0;
  fVar5 = (double)car_probe_ground_distance(*(int *)&param_2,*(int *)&param_3,*(int *)&param_4,param_6);
  pCVar2 = player_car[param_1].contacts;
  iVar4 = 0x12;
  do {
    pCVar2->triangle = -1;
    pCVar2 = pCVar2 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  piVar3 = &player_car[param_1].wheels[0].contact_triangle;
  iVar4 = 4;
  do {
    *piVar3 = -1;
    piVar3 = piVar3 + 0x31;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  player_car[param_1].matrix_current = 0;
  player_car[param_1].matrix_previous = 1;
  player_car[param_1].velocity_history_index = 0;
  fVar1 = player_car[param_1].matrices[3];
  matrix = player_car[param_1].matrices;
  player_car[param_1].matrices[4] = 0.0;
  player_car[param_1].matrices[2] = 0.0;
  player_car[param_1].matrices[1] = 0.0;
  player_car[param_1].matrices[9] = 0.0;
  player_car[param_1].matrices[8] = 0.0;
  player_car[param_1].matrices[6] = 0.0;
  player_car[param_1].matrices[0xe] = 0.0;
  player_car[param_1].matrices[0xd] = 0.0;
  player_car[param_1].matrices[0xc] = 0.0;
  matrix_00 = (RwMatrix *)(player_car[param_1].matrices + 0x10);
  player_car[param_1].matrices[10] = 1.0;
  player_car[param_1].matrices[5] = 1.0;
  *matrix = 1.0;
  *(uint *)&player_car[param_1].matrices[3] = *(uint *)&fVar1 | 0x20003;
  *(uint *)&player_car[param_1].matrices[0x13] = *(uint *)&player_car[param_1].matrices[0x13] | 0x20003;
  player_car[param_1].matrices[0x1a] = 1.0;
  player_car[param_1].matrices[0x15] = 1.0;
  (matrix_00->right).x = 1.0;
  uVar16 = 0;
  translation_00 = &local_c;
  player_car[param_1].matrices[0x14] = 0.0;
  player_car[param_1].matrices[0x12] = 0.0;
  player_car[param_1].matrices[0x11] = 0.0;
  player_car[param_1].matrices[0x19] = 0.0;
  player_car[param_1].matrices[0x18] = 0.0;
  player_car[param_1].matrices[0x16] = 0.0;
  player_car[param_1].matrices[0x1e] = 0.0;
  player_car[param_1].matrices[0x1d] = 0.0;
  player_car[param_1].matrices[0x1c] = 0.0;
  local_c.y = (RwReal)((double)param_3 - fVar5);
  local_c.x = param_2;
  local_c.z = param_4;
  pfVar15 = matrix;
  RwMatrixTranslate((RwMatrix *)matrix,translation_00,rwCOMBINEREPLACE);
  uVar14 = 0;
  translation = &local_c;
  pRVar13 = matrix_00;
  RwMatrixTranslate(matrix_00,translation,rwCOMBINEREPLACE);
  uVar12 = 1;
  puVar10 = (undefined *)&g_car_62ac74;
  pfVar9 = matrix;
  RVar11 = angle;
  RwMatrixRotate((RwMatrix *)matrix,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  uVar8 = 1;
  puVar7 = (undefined *)&g_car_62ac74;
  RwMatrixRotate(matrix_00,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  uVar6 = 0;
  car_wheel_contact_update_r(param_6,(int)(player_car + param_1),0);
  car_set_physics_matrix
            (param_1,(RwMatrix *)matrix,uVar6,matrix_00,puVar7,*(int *)&angle,uVar8,pfVar9,puVar10,*(int *)&RVar11,
             uVar12,pRVar13,translation,uVar14,pfVar15,translation_00,uVar16);
  vehicle_set_active(param_1,1);
  car_reset_state(player_car[param_1].index);
  return 1;
}


