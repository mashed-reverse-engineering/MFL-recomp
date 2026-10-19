#include "mfl_tier2.h"
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0047b520*/ extern int car_current_matrix();
/*005df5c0*/ extern float k_f_49;
/*006a7d78*/ extern undefined4 g_weapon_mines_6a7d78;
static int weapon_fx_slot_clear_b_r(undefined4 * reg_esi);
extern void weapon_fx_slot_clear_b(void);
extern char g_weapon_6a8af8[];
#define RACE_SLOT_COUNT (4)
#define MATRIX_TRANS_X (0x30)
#define MATRIX_TRANS_Y (0x34)
#define MATRIX_TRANS_Z (0x38)
#define MINE_STRIDE (0x1b)
__declspec(naked) int weapon_fx_slot_clear_b_r(undefined4 * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_slot_clear_b
        pop esi
        ret
    }
}

/*
 * Clears the weapon mines that lie within radius 7 (squared distance below k_f_49) of the first
 * used race slot's car (the player's car, inferred). Finds the first used race slot (up to
 * RACE_SLOT_COUNT) and reads that car's position from its matrix
 * (translation row). Walks the mine table in g_weapon_mines_6a7d78 and,
 * for each mine whose state field is 3 (inferred: armed) and whose squared
 * distance to the car is below k_f_49, calls weapon_fx_slot_clear_b_r on it.
 * Takes no parameters and returns nothing.
 */
void weapon_mines_reset_all(void)

{
  int slot_used;
  int slot_index;
  float *mine;
  char *matrix_ptr;
  float car_x;
  float car_y;
  float car_z;

  car_x = 0.0;
  car_y = 0.0;
  car_z = 0.0;
  /* Find the first used race slot; take its car position from its matrix. */
  slot_index = 0;
  do {
    matrix_ptr = 0;
    slot_used = race_setup_slot_used(slot_index);
    if (slot_used != 0) {
      car_current_matrix(&matrix_ptr,slot_index);
      car_x = *(float *)(matrix_ptr + MATRIX_TRANS_X);
      car_y = *(float *)(matrix_ptr + MATRIX_TRANS_Y);
      car_z = *(float *)(matrix_ptr + MATRIX_TRANS_Z);
      break;
    }
    slot_index = slot_index + 1;
  } while (slot_index < RACE_SLOT_COUNT);
  /* Walk the mine table; clear each armed mine close enough to the car. */
  mine = (float *)&g_weapon_mines_6a7d78;
  do {
    if ((*(int *)&mine[8] == 3) &&
       ((mine[2] - car_z) * (mine[2] - car_z) +
        (*mine - car_x) * (*mine - car_x) +
        (mine[1] - car_y) * (mine[1] - car_y) < k_f_49)) {
      weapon_fx_slot_clear_b_r(mine + -0x10);
    }
    mine = mine + MINE_STRIDE;
  } while ((LONG_PTR)mine < ((LONG_PTR)&g_weapon_6a8af8));
  return;
}


