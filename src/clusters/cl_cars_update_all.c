// cluster-entries: cars_update_all
/* Installed cluster cars_update_all (tools/cluster_install.py from output/cluster_pilot_zx/cars_update_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*00475890*/ extern undefined __cdecl car_hit_by_leader(undefined4 p0);
/*004b2b48*/ extern int _ftol();
/*005dda24*/ extern float k_f_0_75;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7c4*/ extern undefined4 g_cars_7ef7c4;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01a0*/ extern undefined1 g_cars_7f01a0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008b06e0*/ extern struct Car player_car[16];
/*008d8b30*/ extern undefined4 player_count;




extern char player_car_0__matrices_12_[];
#define MAX_CARS (16)
#define CAR_SIZE (0xd04) /* sizeof(struct Car) */

/* cars_update_all
 * Per frame physics step for every car slot. For each active, simulated car it integrates the
 * car (with the player input, or no input while the car is controlled by the game), updates the
 * wheel contacts and tests the world collision; then it tests the car against every later car
 * with a bounding sphere and resolves car vs car hits, retrying until no collision is left.
 * Finally it swaps the car's matrix double buffer (current <-> previous).
 * dt: frame time in ms (inferred from the time step use).
 * param_2: not identified; passed in eax to car_wheel_contact_update (inferred).
 */

/* "for the 16 car slots: car_integrate_step + car_wheel_contact_update + car vs car sphere tests,
   retries; swaps the matrix double buffer" */

static bool car_car_collision_r(int p0, int p1, undefined4 p2, int p3);
extern void car_car_collision(void);
static int car_ground_contact_forces_r(int p0);
extern void car_ground_contact_forces(void);
static int car_integrate_step_r(undefined4 * p0, float * p1, float * p2, float p3, byte * p4);
extern void car_integrate_step(void);
static int car_wheel_contact_update_r(int p0, int p1, int p2);
extern void car_wheel_contact_update(void);
static int car_world_collision_test_r(int * p0);
extern void car_world_collision_test(void);
void __cdecl cars_update_all(float dt,undefined4 param_2)

{
  float *own_pos;
  float *other_pos;
  uint cur_matrix;
  uint prev_matrix;
  float collision_range;
  bool no_collision;
  int race_phase;
  int num_players;
  int retry_count;
  int other_index;
  int collided;
  Car *car;
  int other_offset;
  struct {
    PlayerInput *input;
    int car_index;
  } frame;

  race_phase = race_get_phase();
  num_players = get_player_count();
  if ((g_cars_7ef7c4 != 0) && (g_cars_7ef7c4 = ((int)((double)(int)g_cars_7ef7c4 - dt)), (int)g_cars_7ef7c4 < 0)) {
    g_cars_7ef7c4 = 0;
  }
  frame.car_index = 0;
  do {
    /* input of the slot's controller; the neutral input g_cars_7f01a0 when the car is in a
       special mode, has controls disabled or the race phase is not a driving phase (inferred) */
    frame.input = g_player_inputs + g_player_slots[frame.car_index].controller;
    car = player_car + frame.car_index;
    if (player_car[frame.car_index].mode != 0) {
      frame.input = (PlayerInput *)&g_cars_7f01a0;
    }
    if (player_car[frame.car_index].controls_disabled != 0) {
      frame.input = (PlayerInput *)&g_cars_7f01a0;
    }
    retry_count = race_get_phase();
    if (((retry_count != 6) && (retry_count = race_get_phase(), retry_count != 0xb)) &&
       (retry_count = race_get_phase(), retry_count != RACE_PHASE_WINNER_DONE)) {
      frame.input = (PlayerInput *)&g_cars_7f01a0;
    }
    if ((player_car[frame.car_index].rwp_object == 0) &&
       ((player_car[frame.car_index].active != 0 || (player_car[frame.car_index].controls_disabled == 1)))) {
      cur_matrix = player_car[frame.car_index].matrix_current;
      prev_matrix = player_car[frame.car_index].matrix_previous;
      /* retry_count: also the number of collision hits found for this car, the step is
         repeated until no hit is left (at most 2 integrate passes, inferred) */
      retry_count = 0;
      if (player_car[frame.car_index].mode != 0xff) {
        do {
          /* integrate the car, update the wheel contacts and the world collision */
          while( true ) {
            if (1 < retry_count) goto LAB_0047f2d7;
            no_collision = true;
            car_integrate_step_r(car,car->matrices + cur_matrix * 0x10,car->matrices + prev_matrix * 0x10,dt
                               ,&frame.input->left);
            car_wheel_contact_update_r(param_2,car,1);
            other_index = car_world_collision_test_r(car);
            if ((*(int *)&player_car[frame.car_index].field_0x9ec == 0) ||
               (car_ground_contact_forces_r(car), other_index == 0)) break;
            retry_count = retry_count + 1;
          }
          /* car vs car: test every later car slot; other_index holds the world collision
             result until this point (reused, inferred) */
          other_index = frame.car_index + 1;
          if ((MAX_CARS - 1) < other_index) break;
          other_offset = other_index * CAR_SIZE;
          do {
            /* only occupied slots: below num_players and active or with controls disabled */
            if ((other_index < num_players) &&
               ((*(int *)((LONG_PTR)player_car[0].effect_level + other_offset + -0x1c) != 0 ||
                (*(int *)((LONG_PTR)player_car[0].effect_level + other_offset + -0x10) == 1)))) {
              own_pos = car->matrices + player_car[frame.car_index].matrix_current * 0x10 + 0xc;
              other_pos = (float *)(*(int *)((LONG_PTR)player_car[0].velocity + other_offset + -8) * 0x40 +
                                 ((int)&player_car_0__matrices_12_) + other_offset);
              collision_range = (*(float *)((LONG_PTR)player_car[0].contacts + other_offset + -8) +
                      player_car[frame.car_index].bounds_radius) * k_f_0_75;
              /* bounding sphere test: squared distance of the two car positions against
                 (sum of the two bounds radii * 0.75)^2; then the hit must be in a race phase
                 with car vs car collisions (6, 7, 10, 11) */
              if (((*own_pos - *other_pos) * (*own_pos - *other_pos) +
                   (own_pos[1] - other_pos[1]) * (own_pos[1] - other_pos[1]) +
                   (own_pos[2] - other_pos[2]) * (own_pos[2] - other_pos[2]) < collision_range * collision_range) &&
                 (((((race_phase == 6 || (race_phase == 7)) || (race_phase == RACE_PHASE_WINNER_DONE)) ||
                   (race_phase == 0xb)) &&
                  (collided = car_car_collision_r(car,(int)player_car[0].effect_level + other_offset + -0x20,
                                             other_index,retry_count), collided != 0)))) {
                retry_count = retry_count + 1;
                no_collision = false;
                /* in game mode 1 a hit involving the race leader (g_cars_7ef7bc) is reported */
                if (((game_mode == GAME_MODE_HOLD_THE_FLAG) && (g_cars_7ef7c4 == 0)) &&
                   ((collided = other_index, frame.car_index == g_cars_7ef7bc ||
                    (collided = frame.car_index, other_index == g_cars_7ef7bc)))) {
                  car_hit_by_leader(collided);
                }
              }
            }
            other_offset = other_offset + CAR_SIZE;
            other_index = other_index + 1;
          } while (other_offset < (MAX_CARS * CAR_SIZE));
        } while (!no_collision);
        /* swap the matrix double buffer */
        cur_matrix = player_car[frame.car_index].matrix_current;
        player_car[frame.car_index].matrix_previous = cur_matrix;
        player_car[frame.car_index].matrix_current = cur_matrix - 1 & 1;
      }
    }
LAB_0047f2d7:
    frame.car_index = frame.car_index + 1;
    if ((MAX_CARS - 1) < frame.car_index) {
      return;
    }
  } while( true );
}

/* car_car_collision_r: C -> original car_car_collision (registers ecx,eax,s0,s1) */
__declspec(naked) bool car_car_collision_r(int p0, int p1, undefined4 p2, int p3)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call car_car_collision
        add esp, 8
        ret
    }
}

/* car_ground_contact_forces_r: C -> original car_ground_contact_forces (registers edi) */
__declspec(naked) int car_ground_contact_forces_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call car_ground_contact_forces
        pop edi
        ret
    }
}

/* car_integrate_step_r: C -> original car_integrate_step (registers esi,edi,ebx,s0,s1) */
__declspec(naked) int car_integrate_step_r(undefined4 * p0, float * p1, float * p2, float p3, byte * p4)
{
    __asm {
        push ebx
        push esi
        push edi
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        mov esi, dword ptr [esp + 24]
        mov edi, dword ptr [esp + 28]
        mov ebx, dword ptr [esp + 32]
        call car_integrate_step
        add esp, 8
        pop edi
        pop esi
        pop ebx
        ret
    }
}

/* car_wheel_contact_update_r: C -> original car_wheel_contact_update (registers eax,edi,s0) */
__declspec(naked) int car_wheel_contact_update_r(int p0, int p1, int p2)
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

/* car_world_collision_test_r: C -> original car_world_collision_test (registers esi) */
__declspec(naked) int car_world_collision_test_r(int * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call car_world_collision_test
        pop esi
        ret
    }
}
