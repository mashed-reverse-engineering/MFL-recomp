#include "mfl_tier2.h"
#define COURSE_ROUABOUT (2)
#define COURSE_WARZONE (33)
typedef int CourseId;
/*0040fdc0*/ extern int race_get_phase();
/*00422120*/ extern int vehicle_get_matrix_body(int car, void *out);
/*0042b110*/ extern int current_course_id();
/*00464e30*/ extern int powerups_update();
/*0045f650*/ extern void explosion_queue_update(void);
/*00465fe0*/ extern int weapon_effects_particles_update();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00467ed0*/ extern int player_equip_weapon_by_index(int player, int weapon);
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(float *hit, float *position);
/*004866d0*/ extern int fx_emitter_spawn_b_default(float *position, void *style, undefined4 size, float speed);
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4c70*/ extern int FUN_004c4c70(undefined4 rw_object, float *position, float *hit);
/*0060dc3c*/ extern void *weapon_descriptor_update_callbacks;
/*0060dc5c*/ extern undefined g_weapons_update_60dc5c;
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006aac18*/ extern undefined g_player_status;
/*0062ac74*/ extern undefined g_car_62ac74;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
static int player_unequip_weapon_r(int reg_esi);
extern void player_unequip_weapon(void);
__declspec(naked) int player_unequip_weapon_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call player_unequip_weapon
        pop esi
        ret
    }
}

/* offsets inside a 0xb4 byte weapon slot */
#define SLOT_POSITION (0x30)      /* body position, written by vehicle_get_matrix_body(car, slot) */
#define SLOT_POSITION_B (0x70)      /* second position, written by vehicle_get_matrix_body(car, slot + 0x40) */
#define SLOT_AIM (0x80)
#define SLOT_MOVE_A (0x90)      /* position change since the last frame */
#define SLOT_MOVE_B (0x9c)
#define SLOT_DESCRIPTOR (0xa8)      /* weapon descriptor: +8 fire(slot, state), +0xc update(slot) */
#define SLOT_ARMED (0xac)
#define SLOT_SIZE (0xb4)
#define DESC_FIRE (0x8)
#define DESC_UPDATE (0xc)
#define INPUT_FIRE 7
#define INPUT_DROP 8
#define INPUT_EQUIP (0x1d)
/* weapons_update_frame */

/* "per frame: for each player slot (0xB4 bytes at player_weapon_slots) in phase 6 and status != 4:
   status 2 -> 3 and unequip; if a weapon is equipped call descriptor+0xC (update, nonzero =
   finished -> unequip), derive a fire state from input byte +7 and the previous frame's copy (3
   pressed with no previous / 2 / 1 released) and call descriptor+8(slot, state); then the per type
   per frame callbacks (descriptor +0x1C, and +0x38/+0x3C for course ids 2 and 0x21), FUN_00465fe0,
   powerups_update and the delayed trigger spheres (explosion_queue_update, tail call)" */

void weapons_update_frame(void)

{
  int race_phase;
  int slot_index;
  int fire_state;
  int index;
  int hit;
  int controller;
  char *slot;
  char *descriptor;
  void (__cdecl *callback)(void);
  float old_a[3];
  float old_b[3];
  float hit_position[3];
  
  race_phase = race_get_phase();
  slot_index = 0;
  slot = (char *)player_weapon_slots;
  do {
    old_a[0] = *(float *)(slot + SLOT_POSITION);
    old_a[1] = *(float *)(slot + SLOT_POSITION + 4);
    old_a[2] = *(float *)(slot + SLOT_POSITION + 8);
    old_b[0] = *(float *)(slot + SLOT_POSITION_B);
    old_b[1] = *(float *)(slot + SLOT_POSITION_B + 4);
    old_b[2] = *(float *)(slot + SLOT_POSITION_B + 8);
    fire_state = 0;
    vehicle_get_matrix_body(slot_index,slot);
    vehicle_get_matrix_body(slot_index,slot + 0x40);
    *(float *)(slot + SLOT_MOVE_A) = *(float *)(slot + SLOT_POSITION) - old_a[0];
    *(float *)(slot + SLOT_MOVE_A + 4) = *(float *)(slot + SLOT_POSITION + 4) - old_a[1];
    *(float *)(slot + SLOT_MOVE_A + 8) = *(float *)(slot + SLOT_POSITION + 8) - old_a[2];
    *(float *)(slot + SLOT_MOVE_B) = *(float *)(slot + SLOT_POSITION_B) - old_b[0];
    *(float *)(slot + SLOT_MOVE_B + 4) = *(float *)(slot + SLOT_POSITION_B + 4) - old_b[1];
    *(float *)(slot + SLOT_MOVE_B + 8) = *(float *)(slot + SLOT_POSITION_B + 8) - old_b[2];
    if ((race_phase == 6) && (*(int *)((char *)&g_player_status + slot_index * 4) != 4)) {
      if (*(int *)((char *)&g_player_status + slot_index * 4) == 2) {
        *(int *)((char *)&g_player_status + slot_index * 4) = 3;
        if (*(int *)(slot + SLOT_DESCRIPTOR) != 0) {
          player_unequip_weapon_r((int)slot);
        }
      }
      else if (*(int *)((char *)&g_player_status + slot_index * 4) != 3) {
        if (*(int *)(slot + SLOT_ARMED) != 0) {
          /* probe the world from the body position: a miss unequips the weapon and spawns an emitter */
          *(undefined4 *)(slot + SLOT_AIM) = *(undefined4 *)(slot + SLOT_POSITION);
          *(undefined4 *)(slot + SLOT_AIM + 4) = *(undefined4 *)(slot + SLOT_POSITION + 4);
          *(undefined4 *)(slot + SLOT_AIM + 8) = *(undefined4 *)(slot + SLOT_POSITION + 8);
          hit = FUN_004c4c70(weapon_get_rw_801b14(),(float *)(slot + SLOT_AIM),hit_position);
          if ((hit != 0) && (rocket_surface_hit_test(hit_position,(float *)(slot + SLOT_AIM)) == 0)) {
            player_unequip_weapon_r((int)slot);
            fx_emitter_spawn_b_default((float *)(slot + SLOT_AIM),&g_car_62ac74,0x43b40000,
                                       rp_random_float_range(0x3fa00000,0x3fe00000));
          }
        }
        controller = g_player_slots[slot_index].controller;
        if (*(int *)(slot + SLOT_ARMED) == 0) {
          /* nothing equipped: the equip button picks weapon 0xc */
          if ((*((char *)&g_player_inputs[controller] + INPUT_EQUIP) != '\0') &&
             (*((char *)&g_player_inputs_previous[controller] + INPUT_EQUIP) == '\0')) {
            player_equip_weapon_by_index(slot_index,0xc);
          }
        }
        else {
          descriptor = *(char **)(slot + SLOT_DESCRIPTOR);
          if ((*(int (__cdecl **)(char *))(descriptor + DESC_UPDATE))(slot) != 0) {
            player_unequip_weapon_r((int)slot);
          }
          else {
            /* fire state: 3 held (no previous), 2 pressed, 1 released, 0 nothing */
            if (*((char *)&g_player_inputs[controller] + INPUT_FIRE) != '\0') {
              fire_state = 3;
            }
            if (*((char *)&g_player_inputs_previous[controller] + INPUT_FIRE) != '\0') {
              if (*((char *)&g_player_inputs[controller] + INPUT_FIRE) == '\0') {
                fire_state = 1;
              }
            }
            else if (*((char *)&g_player_inputs[controller] + INPUT_FIRE) != '\0') {
              fire_state = 2;
            }
            if (fire_state != 0) {
              (*(void (__cdecl **)(char *,int))(descriptor + DESC_FIRE))(slot,fire_state);
            }
            else if ((*((char *)&g_player_inputs[controller] + INPUT_DROP) != '\0') &&
                    (*((char *)&g_player_inputs_previous[controller] + INPUT_DROP) == '\0')) {
              player_unequip_weapon_r((int)slot);
            }
          }
        }
      }
    }
    slot = slot + SLOT_SIZE;
    slot_index = slot_index + 1;
  } while (slot_index < 4);
  index = 0;
  if (0 < (int)weapon_descriptor_count) {
    descriptor = (char *)&weapon_descriptor_update_callbacks;
    do {
      (**(void (__cdecl **)(void))descriptor)();
      index = index + 1;
      descriptor = descriptor + 0x40;
    } while (index < (int)weapon_descriptor_count);
  }
  index = 0;
  if (0 < (int)weapon_descriptor_count) {
    descriptor = (char *)&g_weapons_update_60dc5c;
    do {
      race_phase = current_course_id();
      callback = (void (__cdecl *)(void))0;
      if (race_phase == COURSE_ROUABOUT) {
        callback = *(void (__cdecl **)(void))descriptor;
      }
      else if (race_phase == COURSE_WARZONE) {
        callback = *(void (__cdecl **)(void))(descriptor - 4);
      }
      if (callback != (void (__cdecl *)(void))0) {
        (*callback)();
      }
      index = index + 1;
      descriptor = descriptor + 0x40;
    } while (index < (int)weapon_descriptor_count);
  }
  weapon_effects_particles_update();
  powerups_update();
  explosion_queue_update();
  return;
}
