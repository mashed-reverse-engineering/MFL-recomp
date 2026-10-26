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
  float flags_word;
  ContactPoint *contact;
  int *wheel_tri;
  int count;
  double ground_dist;
  undefined4 stale_a;
  RwMatrix *mtx_b;
  undefined *stale_b;
  undefined4 stale_c;
  float *stale_d;
  undefined *stale_e;
  RwReal stale_f;
  undefined4 stale_g;
  RwMatrix *stale_h;
  RwV3d *stale_i;
  undefined4 stale_j;
  float *stale_k;
  RwV3d *translation;
  undefined4 stale_l;
  RwV3d ground_pos;
  
  if (0xf < car_index) {
    return 0;
  }
  player_car[car_index].mode = 0;
  player_car[car_index].mode2 = 0;
  ground_dist = (double)car_probe_ground_distance(*(int *)&x,*(int *)&y,*(int *)&z,probe_arg);
  contact = player_car[car_index].contacts;
  count = 0x12;
  do {
    contact->triangle = -1;
    contact = contact + 1;
    count = count + -1;
  } while (count != 0);
  wheel_tri = &player_car[car_index].wheels[0].contact_triangle;
  count = 4;
  do {
    *wheel_tri = -1;
    wheel_tri = wheel_tri + 0x31;
    count = count + -1;
  } while (count != 0);
  player_car[car_index].matrix_current = 0;
  player_car[car_index].matrix_previous = 1;
  player_car[car_index].velocity_history_index = 0;
  flags_word = player_car[car_index].matrices[3];
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
  *(uint *)&player_car[car_index].matrices[3] = *(uint *)&flags_word | 0x20003;
  *(uint *)&player_car[car_index].matrices[0x13] = *(uint *)&player_car[car_index].matrices[0x13] | 0x20003;
  player_car[car_index].matrices[0x1a] = 1.0;
  player_car[car_index].matrices[0x15] = 1.0;
  (mtx_b->right).x = 1.0;
  stale_l = 0;
  translation = &ground_pos;
  player_car[car_index].matrices[0x14] = 0.0;
  player_car[car_index].matrices[0x12] = 0.0;
  player_car[car_index].matrices[0x11] = 0.0;
  player_car[car_index].matrices[0x19] = 0.0;
  player_car[car_index].matrices[0x18] = 0.0;
  player_car[car_index].matrices[0x16] = 0.0;
  player_car[car_index].matrices[0x1e] = 0.0;
  player_car[car_index].matrices[0x1d] = 0.0;
  player_car[car_index].matrices[0x1c] = 0.0;
  ground_pos.y = (RwReal)((double)y - ground_dist);
  ground_pos.x = x;
  ground_pos.z = z;
  stale_k = mtx_a;
  RwMatrixTranslate((RwMatrix *)mtx_a,translation,rwCOMBINEREPLACE);
  stale_j = 0;
  stale_i = &ground_pos;
  stale_h = mtx_b;
  RwMatrixTranslate(mtx_b,stale_i,rwCOMBINEREPLACE);
  stale_g = 1;
  stale_e = (undefined *)&g_car_62ac74;
  stale_d = mtx_a;
  stale_f = angle;
  RwMatrixRotate((RwMatrix *)mtx_a,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  stale_c = 1;
  stale_b = (undefined *)&g_car_62ac74;
  RwMatrixRotate(mtx_b,(RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
  /* stale_a..stale_l: leftover argument temporaries of the decompiler (extra values passed to car_set_physics_matrix / the RwMatrix calls); they do not carry meaning */
  stale_a = 0;
  car_wheel_contact_update_r(probe_arg,(int)(player_car + car_index),0);
  car_set_physics_matrix
            (car_index,(RwMatrix *)mtx_a,stale_a,mtx_b,stale_b,*(int *)&angle,stale_c,stale_d,stale_e,*(int *)&stale_f,
             stale_g,stale_h,stale_i,stale_j,stale_k,translation,stale_l);
  vehicle_set_active(car_index,1);
  car_reset_state(player_car[car_index].index);
  return 1;
}


