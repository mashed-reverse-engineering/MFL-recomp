#include "mfl_tier2.h"
#define COURSE_ARCTIC (0)
#define COURSE_TIERRA_N (6)
#define COURSE_SUPERG_N (8)
#define COURSE_HIGHWAYN (12)
#define COURSE_CITY (26)
#define COURSE_WARZONE (33)
#define COURSE_STORM (36)
typedef int CourseId;
/*00409a70*/ extern undefined __cdecl player_time_update(undefined4 p0, undefined4 p1, undefined4 p2);
/*0040f3f0*/ extern int car_boost_flame_effects_update();
/*00421f30*/ extern int vehicle_set_flag_40();
/*004237d0*/ extern int vehicle_setup_references();
/*0042a410*/ extern int course_get_ai_world();
/*0042b110*/ extern int current_course_id();
/*0047a7f0*/ extern int __cdecl car_get_controls_disabled(undefined4 p0);
/*0047a830*/ extern int car_is_active();
/*0047b520*/ extern int car_current_matrix();
/*0050b5e0*/ extern int nop_stub();
/*0065314c*/ extern undefined4 g_car_65314c;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b14*/ extern undefined4 g_race_setup;
/*007f023c*/ extern char g_ai_editor_mode[];
/* byte offset in g_race_setup of the per-slot "car in use" words, one per slot (inferred) */
#define RACE_SETUP_SLOT_ACTIVE_OFFSET (0xb8)
/* cars_update_per_frame_setup */

/* "loop over the four slots: vehicle references, flame effects, per course flag g_car_65314c,
   player_time_update" */

/*
 * Per-frame car update for the four player slots.
 * For every slot whose RACE_SETUP_SLOT_ACTIVE_OFFSET word in g_race_setup is non-zero:
 *   - if the car is active (or its controls are disabled), refresh its matrix, set up its
 *     references, update the boost flame effects and run player_time_update;
 *   - otherwise clear the slot's g_car_65314c flag.
 * Afterwards vehicle_set_flag_40 is called with the slot's flag and the brake state of its
 * controller (taken from g_player_inputs).
 * frame_context is passed through to vehicle_setup_references (inferred: per-frame setup
 * context). Returns nothing.
 */
void cars_update_per_frame_setup(undefined4 frame_context)

{
  undefined4 *car_slot;
  int car_active;
  CourseId course_id;
  undefined4 ai_world;
  PlayerSlot *player_slot;
  int slot_index;
  bool brake_held;
  undefined4 car_slots [4];

  brake_held = false;
  nop_stub();
  slot_index = 0;
  player_slot = g_player_slots;
  /* loop over the player slots (the end bound is the address just past g_player_slots) */
  do {
    car_slot = car_slots + slot_index;
    /* skip slots whose "car in use" word in g_race_setup is zero */
    if (*(int *)((LONG_PTR)car_slot + g_race_setup + (RACE_SETUP_SLOT_ACTIVE_OFFSET - (LONG_PTR)car_slots)) != 0) {
      car_active = car_is_active(slot_index);
      /* car is active, or its controls are disabled: full update */
      if ((car_active == 1) || (car_active = car_get_controls_disabled(slot_index), car_active == 1)) {
        car_current_matrix(car_slot,slot_index);
        vehicle_setup_references(slot_index,frame_context,*car_slot);
        car_boost_flame_effects_update(slot_index);
        brake_held = g_player_inputs[player_slot->controller].brake != 0;
        course_id = current_course_id();
        /* g_car_65314c[slot] = 1 on the courses listed below, 0 on every other course */
        switch(course_id) {
        case COURSE_ARCTIC:
        case COURSE_TIERRA_N:
        case COURSE_SUPERG_N:
        case COURSE_HIGHWAYN:
        case COURSE_CITY:
        case COURSE_WARZONE:
        case COURSE_STORM:
          (&g_car_65314c)[slot_index] = 1;
          break;
        default:
          (&g_car_65314c)[slot_index] = 0;
        }
        /* car_active is reused here to carry the slot index into player_time_update */
        car_active = slot_index;
        ai_world = course_get_ai_world();
        player_time_update(*car_slot,ai_world,car_active);
      }
      /* car inactive and controls not disabled: clear its flag */
      else {
        (&g_car_65314c)[slot_index] = 0;
      }
      vehicle_set_flag_40(slot_index,(&g_car_65314c)[slot_index],(&g_car_65314c)[slot_index],brake_held);
    }
    player_slot = player_slot + 1;
    slot_index = slot_index + 1;
  } while ((LONG_PTR)player_slot < ((LONG_PTR)&g_ai_editor_mode));
  return;
}


