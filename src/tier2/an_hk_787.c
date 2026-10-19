#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];

/* race phase in which rumble is sent (inferred: only phase that passes the gate) */
#define RUMBLE_RACE_PHASE (6)

/* player_rumble */

/* Queues a rumble request on the controller of one player slot (inferred from the body).
   slot: index into g_player_slots (a player slot, not a car).
   motor: rumble motor, 1..3 (dossier).
   duration: third argument of callers, 2 / 10 / 0x1e (dossier).
   strength: fourth argument of callers, 0x80 / 0xff (dossier).
   Returns nothing. Only written when the slot is type 1, the race phase is RUMBLE_RACE_PHASE
   and the controller has rumble enabled; the input backend consumes rumble_request.
   scratch: reused for the slot type, then the race phase, then the controller index.
   (was: "(car, motor, 0xF, strength): sets the force feedback fields of the player's input struct when it
   has a controller and the race phase is 6") */

void player_rumble(int slot,uint motor,uint duration,uint strength)

{
  int scratch;

  /* slot type must be 1 (meaning of type 1 not in dossier) */
  scratch = race_setup_get_slot_type(slot);
  if (scratch == 1) {
    /* only during the rumble race phase */
    scratch = race_get_phase();
    if (scratch == RUMBLE_RACE_PHASE) {
      /* scratch is now the PlayerInput index of this slot's controller */
      scratch = g_player_slots[slot].controller;
      /* controller must have rumble enabled; then write the request */
      if (g_player_inputs[scratch].rumble_enabled != 0) {
        g_player_inputs[scratch].rumble_motor = motor;
        g_player_inputs[scratch].rumble_request = 1;
        g_player_inputs[scratch].rumble_duration = duration;
        g_player_inputs[scratch].rumble_strength = strength;
      }
    }
  }
  return;
}


