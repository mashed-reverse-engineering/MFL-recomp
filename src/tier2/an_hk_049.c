#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
/*0047a830*/ extern int car_is_active();
/*0047b520*/ extern int car_current_matrix();
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, void *p1);
/*0056bc20*/ extern void rwp_body_add_force_at_point(undefined4 p0, void *p1, void *p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005ddea0*/ extern float k_f_20;
/*005de83c*/ extern float k_f_n20;
/*0062ac74*/ extern float g_car_62ac74;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008b06e0*/ extern struct Car player_car[16];

/* bit index in a 32 bit word table: word = index >> 5, bit = index & 0x1f */
#define BITSET_WORD_SHIFT (5)
#define BITSET_BIT_MASK (0x1f)


/* car_apply_throttle_force */

/* "when wheels touch the ground applies +-20 forward force by accelerate/brake input to the body"
    */

/* Applies the throttle (accelerate) or brake force to car slot car_index (0..15 of player_car).
   The force is only applied while the car is active, has a physics body and at least one wheel
   is in contact with the ground. The input comes from the controller bound to the car's slot
   in g_player_slots. Accelerate pushes along the car's forward vector with +k_f_20, brake with
   k_f_n20; both are applied at the car's matrix position. Returns nothing. */
void __cdecl car_apply_throttle_force(int car_index)

{
  int controller_index;
  char *car_active;
  PlayerInput *player_input;
  char contact_count;
  RwMatrix *car_matrix;
  PlayerInput *controller_input;
  RwV3d car_pos;   /* two 12 byte vectors in one object each: rwp_body_add_force_at_point reads 3 floats through each pointer */
  RwV3d drive_force;
  
  /* controller_index: PlayerInput index bound to this car's slot (-1 = no player) */
  controller_index = g_player_slots[car_index].controller;
  /* count the wheels that touch the ground (non-zero field at Wheel +0x34), 0..4 */
  contact_count = *(int *)((char *)&player_car[car_index].wheels[0] + 0x34) != 0;
  if (*(int *)((char *)&player_car[car_index].wheels[1] + 0x34) != 0) {
    contact_count = contact_count + '\x01';
  }
  if (*(int *)((char *)&player_car[car_index].wheels[2] + 0x34) != 0) {
    contact_count = contact_count + '\x01';
  }
  if (*(int *)((char *)&player_car[car_index].wheels[3] + 0x34) != 0) {
    contact_count = contact_count + '\x01';
  }
  controller_input = g_player_inputs + controller_index;
  /* car_active: the physics library object that owns this car (reused as the car_is_active flag below) */
  car_active = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
  car_matrix = *(RwMatrix **)(*(char **)(car_active + 0x10) + 8);
  RwV3dTransformVectors((RwV3d *)player_car[car_index].up,(RwV3d *)&g_car_62ac74,1,car_matrix);
  player_input = g_player_inputs + controller_index;
  /* if the object's bit table has no bit for this car (bit index at object +0x20), register it with FUN_0056b300 and
     set rwp_world_set_bit_5c (inferred: the body is set up on first use) */
  if ((*(int *)(car_active + 0x24) == 0) ||
     (player_input = controller_input,
     (*(uint *)(*(char **)(*(char **)(car_active + 0x24) + 0x60) +
               (uint)(*(ushort *)(car_active + 0x20) >> BITSET_WORD_SHIFT) * 4) &
     1 << ((byte)*(ushort *)(car_active + 0x20) & BITSET_BIT_MASK)) == 0)) {
    FUN_0056b300(*(undefined4 *)(car_active + 0x24),car_active);
    rwp_world_set_bit_5c(*(undefined4 *)(car_active + 0x24),car_active,1);
  }
  car_current_matrix(&car_matrix,car_index);
  /* reused as the body handle of the physics object (inferred), passed to rwp_body_add_force_at_point as its first argument */
  controller_index = *(int *)(*(char **)(car_active + 0x10) + 4);
  /* only apply the force while the car is active and a wheel touches the ground */
  if (((controller_index != 0) && (car_active = car_is_active(car_index), car_active != 0)) && (contact_count != '\0')) {
    /* accelerate: +k_f_20 along the forward vector */
    if (player_input->accelerate != 0) {
      car_pos.x = (car_matrix->pos).x;
      car_pos.y = (car_matrix->pos).y;
      drive_force.x = player_car[car_index].forward[0] * k_f_20;
      car_pos.z = (car_matrix->pos).z;
      drive_force.y = player_car[car_index].forward[1] * k_f_20;
      drive_force.z = player_car[car_index].forward[2] * k_f_20;
      rwp_body_add_force_at_point(controller_index,&drive_force,&car_pos);
    }
    /* brake: k_f_n20 (-20) along the forward vector */
    if (player_input->brake != 0) {
      car_pos.x = (car_matrix->pos).x;
      car_pos.y = (car_matrix->pos).y;
      drive_force.x = player_car[car_index].forward[0] * k_f_n20;
      car_pos.z = (car_matrix->pos).z;
      drive_force.y = player_car[car_index].forward[1] * k_f_n20;
      drive_force.z = player_car[car_index].forward[2] * k_f_n20;
      rwp_body_add_force_at_point(controller_index,&drive_force,&car_pos);
    }
  }
  return;
}
