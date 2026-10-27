#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0041b9d0*/ extern int hudpanel_slot_init_player();
/*004246f0*/ extern int car_set_damage();
/*00467890*/ extern int player_status_set();
/*0047a640*/ extern int car_deactivate();
/*0047a810*/ extern int car_set_controls_disabled();
/*007ef7b8*/ extern undefined4 game_mode;

#define MAX_CAR_SLOTS 0x10
#define CONTROLS_DISABLED 1
#define CAR_KILL_DAMAGE 0x42480000 /* 50.0f */
#define CAR_DAMAGE_SIDE_0 0        /* side 0 (unknown side) */
#define CAR_DAMAGE_SIDE_1 1        /* side 1 (unknown side) */
#define PLAYER_STATUS_WEAPON_2 2   /* weapon status 2 (unknown status name) */
#define SLOT_TYPE_HUMAN 1          /* human player slot type (unknown enum) */
#define HUD_INIT_PLAYER 1          /* player HUD initialization flag (unknown) */


/* car_kill */

/* "(car): car_init, inactive, damage set to 50.0 on both sides, weapon status 2, no HUD in game
   mode 7" */
/* Deactivates and kills a car slot, applying fatal damage and resetting the player HUD if applicable. */
undefined4 car_kill(int car_index)
{
  int slot_type;
  
  if (car_index < MAX_CAR_SLOTS) {
    /* Deactivate the vehicle and disable controls */
    car_deactivate(car_index);
    car_set_controls_disabled(car_index,CONTROLS_DISABLED);
    /* Set 50.0 damage on both sides */
    car_set_damage(car_index,CAR_KILL_DAMAGE,CAR_DAMAGE_SIDE_0);
    car_set_damage(car_index,CAR_KILL_DAMAGE,CAR_DAMAGE_SIDE_1);
    /* Set player weapon status */
    player_status_set(car_index,PLAYER_STATUS_WEAPON_2);
    /* Reinitialize HUD for human player unless running in game mode 7 */
    if (game_mode != GAME_MODE_AIRSTRIKE) {
      slot_type = race_setup_get_slot_type(car_index);
      if (slot_type == SLOT_TYPE_HUMAN) {
        hudpanel_slot_init_player(car_index,HUD_INIT_PLAYER);
      }
    }
  }
  return 0;
}
