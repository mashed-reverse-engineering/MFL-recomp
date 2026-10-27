#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
/*008b06e0*/ extern struct Car player_car[16];

#define MAX_CAR_INDEX (15)
#define ENTRY_COUNT (16)
#define CONTACT_POINTS_PER_CAR (18)
#define FLOAT_BITS_NEG_ONE (0xbf800000)


/* car_reset_state */

/*
 * Zeroes the run time state of one car slot (called by car_spawn).
 * car_index: slot in player_car[], 0..MAX_CAR_INDEX.
 * Returns 1 when the slot was reset, 0 when car_index is out of range.
 */

undefined4 car_reset_state(uint car_index)

{
  undefined4 *entry;
  ContactPoint *contact;
  int remaining;

  if (MAX_CAR_INDEX < car_index) {
    return 0;
  }
  /* wheels 0..2 (front left, front right, rear left): clear the contact state, then mark them in contact */
  player_car[car_index].index = car_index;
  player_car[car_index].wheels[0].field_0x38 = 0;
  player_car[car_index].wheels[0].spin_rate = 0;
  *(undefined4 *)((char *)&player_car[car_index].wheels[0] + 0x40) = 0;
  player_car[car_index].wheels[0].contact_distance = 0.0;
  player_car[car_index].wheels[0].contact_normal[0] = 0.0;
  player_car[car_index].wheels[0].contact_normal[2] = 0.0;
  player_car[car_index].wheels[1].field_0x38 = 0;
  player_car[car_index].wheels[1].spin_rate = 0;
  *(undefined4 *)((char *)&player_car[car_index].wheels[1] + 0x40) = 0;
  player_car[car_index].wheels[1].contact_distance = 0.0;
  player_car[car_index].wheels[1].contact_normal[0] = 0.0;
  player_car[car_index].wheels[1].contact_normal[2] = 0.0;
  player_car[car_index].wheels[2].field_0x38 = 0;
  player_car[car_index].wheels[2].spin_rate = 0;
  *(undefined4 *)((char *)&player_car[car_index].wheels[2] + 0x40) = 0;
  player_car[car_index].wheels[2].contact_distance = 0.0;
  player_car[car_index].wheels[2].contact_normal[0] = 0.0;
  player_car[car_index].wheels[2].contact_normal[2] = 0.0;
  /* contact normal of wheels 0..2 points up (0 1 0) after reset */
  player_car[car_index].wheels[0].contact_normal[1] = 1.0;
  player_car[car_index].wheels[1].contact_normal[1] = 1.0;
  player_car[car_index].wheels[2].contact_normal[1] = 1.0;
  player_car[car_index].wheels[0].in_contact = 1;
  player_car[car_index].wheels[1].in_contact = 1;
  player_car[car_index].wheels[2].in_contact = 1;
  /* wheel 3 (rear right): same reset */
  player_car[car_index].wheels[3].contact_normal[1] = 1.0;
  player_car[car_index].wheels[3].field_0x38 = 0;
  player_car[car_index].wheels[3].spin_rate = 0;
  *(undefined4 *)((char *)&player_car[car_index].wheels[3] + 0x40) = 0;
  player_car[car_index].wheels[3].in_contact = 1;
  player_car[car_index].wheels[3].contact_distance = 0.0;
  player_car[car_index].wheels[3].contact_normal[0] = 0.0;
  player_car[car_index].wheels[3].contact_normal[2] = 0.0;
  player_car[car_index].wheels_in_contact = 4.0;
  *(undefined4 *)&player_car[car_index].force_accum[2] = 0;
  *(undefined4 *)&player_car[car_index].force_accum[1] = 0;
  *(undefined4 *)&player_car[car_index].force_accum[0] = 0;
  *(undefined4 *)((char *)&player_car[car_index] + 0xaf0) = 0;
  player_car[car_index].field_af4[0] = 0.0;
  player_car[car_index].field_af4[1] = -1.0;
  player_car[car_index].field_af4[2] = 0.0;
  /* table of ENTRY_COUNT entries, 3 words each, starting at +0xb30 (inferred): 0, -1.0f, 0 */
  entry = (undefined4 *)&*(undefined4 *)((char *)&player_car[car_index] + 0xb30);
  remaining = ENTRY_COUNT;
  do {
    entry[-1] = 0;
    *entry = FLOAT_BITS_NEG_ONE;
    entry[1] = 0;
    entry = entry + 3;
    remaining = remaining + -1;
  } while (remaining != 0);
  *(undefined4 *)((char *)&player_car[car_index] + 0xbec) = 0;
  /* angular velocity, then linear velocity */
  player_car[car_index].angular_velocity[2] = 0.0;
  player_car[car_index].angular_velocity[1] = 0.0;
  player_car[car_index].angular_velocity[0] = 0.0;
  player_car[car_index].velocity[2] = 0.0;
  player_car[car_index].velocity[1] = 0.0;
  player_car[car_index].velocity[0] = 0.0;
  /* velocity history: 2 samples x 3 floats */
  player_car[car_index].velocity_history[5] = 0.0;
  player_car[car_index].velocity_history[4] = 0.0;
  player_car[car_index].velocity_history[3] = 0.0;
  player_car[car_index].velocity_history[2] = 0.0;
  player_car[car_index].velocity_history[1] = 0.0;
  player_car[car_index].velocity_history[0] = 0.0;
  player_car[car_index].angular_speed = 0.0;
  player_car[car_index].speed = 0.0;
  player_car[car_index].field_b00[2] = 0.0;
  player_car[car_index].field_b00[1] = 0.0;
  player_car[car_index].field_b00[0] = 0.0;
  player_car[car_index].contact_count = 0;
  player_car[car_index].mode = 0;
  player_car[car_index].mode2 = 0;
  player_car[car_index].velocity_history_index = 0;
  player_car[car_index].gear = 0;
  player_car[car_index].gear_shift_timer = 0;
  *(undefined4 *)((char *)&player_car[car_index] + 0xb24) = 0;
  *(undefined4 *)((char *)&player_car[car_index] + 0xb28) = 0;
  *(undefined4 *)((char *)&player_car[car_index] + 0xaec) = 0;
  player_car[car_index].field_b0c = 0;
  player_car[car_index].field_b10 = 0;
  /* no track triangle touched (-1) for every contact point */
  contact = player_car[car_index].contacts;
  remaining = CONTACT_POINTS_PER_CAR;
  do {
    contact->triangle = -1;
    contact = contact + 1;
    remaining = remaining + -1;
  } while (remaining != 0);
  player_car[car_index].rwp_object = 0;
  player_car[car_index].rwp_timer = 0;
  *(undefined4 *)((char *)&player_car[car_index] + 0x1c) = 0;
  /* effect levels, then the hold times that decay them */
  player_car[car_index].effect_level[5] = 0;
  player_car[car_index].effect_level[4] = 0;
  player_car[car_index].effect_level[3] = 0;
  player_car[car_index].effect_level[2] = 0;
  player_car[car_index].effect_level[1] = 0;
  player_car[car_index].effect_level[0] = 0;
  player_car[car_index].effect_hold[5] = 0.0;
  player_car[car_index].effect_hold[4] = 0.0;
  player_car[car_index].effect_hold[3] = 0.0;
  player_car[car_index].effect_hold[2] = 0.0;
  player_car[car_index].effect_hold[1] = 0.0;
  player_car[car_index].effect_hold[0] = 0.0;
  return 1;
}
