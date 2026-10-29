#include "mfl_tier2.h"
/*004089e0*/ extern int fx_object_get_rwp_id();
/*00465470*/ extern int weapon_find_targets_in_cone();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00465f60*/ extern char *weapon_effect_get_slot_record();
/*00484680*/ extern int object_set_flag_4();
/*00486c40*/ extern int particles_flush();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd378*/ extern float k_f_0_1;
/*005dd420*/ extern float k_f_n3;
/*005ddacc*/ extern float k_f_360;
/*005e64a4*/ extern float k_f_0;
/*005de20c*/ extern float k_f_2_5;
/*005de270*/ extern float k_f_n0_1;
/*005df470*/ extern float k_f_n2_5;
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/*006a61f0*/ extern undefined g_weapon_6a61f0;
/*006a6230*/ extern undefined4 g_weapon_6a6230;
/*006a6234*/ extern undefined g_weapon_machinegun_6a6234;
/*007ef7f4*/ extern float g_tick_seconds;

#define MACHINEGUN_COUNT (4)
#define MACHINEGUN_ENTRY_INTS (0x12) /* sizeof(struct MachinegunState) / 4 */
#define SLOT_VALUE_NONE (-1) /* (inferred) "no slot value" from weapon_effect_get_slot_value_b */

/* weapon_machinegun_update */


/*
 * Per-frame update of the machine gun turrets: MACHINEGUN_COUNT entries of
 * struct MachinegunState, walked from the last entry to the first (inferred).
 * For each active gun:
 *   - clears the flag on the gun-part objects at +0x2c, +0x30 and +0x44 (inferred)
 *   - moves the base frame to the slot transform
 *   - finds targets in a cone from the base position, colour from the slot values
 *   - aims: rates from the aim point (in the elevation mount's space), clamped to +/-3,
 *     added to the yaw and elevation angles, wrapped to [0, 360)
 *   - spins the barrel: spin decays by 1.0 per call and feeds the barrel roll angle
 *   - counts the cooldown down by g_tick_seconds
 * Returns nothing; the particle list is flushed at the end.
 */
void weapon_machinegun_update(void)

{
  float tmp_f;
  int slot_value;
  int rwp_id;
  undefined1 *cone_colour;
  RwMatrix *ltm;
  RwReal *aim_record;
  int *cursor;
  int *next_cursor;
  int gun_index;
  undefined4 cone_length;
  undefined4 cone_radius;
  /* Per-update scratch: RGBA cone colours (inferred), aim vectors, gun-inverse matrix */
  struct {
    float pitch_step;
    undefined1 colour_a_r;
    undefined1 colour_a_g;
    undefined1 colour_a_b;
    undefined1 colour_a_a;
    float yaw_step;
    undefined1 colour_b_r;
    undefined1 colour_b_g;
    undefined1 colour_b_b;
    undefined1 colour_b_a;
    RwV3d aim_local;
    float yaw_rate_dead;
    RwV3d axis_y;
    RwV3d axis_x;
    RwV3d axis_z;
    RwV3d aim_point;
    RwMatrix gun_inverse_ltm;
  } scratch;

  gun_index = MACHINEGUN_COUNT;
  scratch.axis_x.x = 1.0;
  scratch.axis_x.y = 0.0;
  scratch.axis_x.z = 0.0;
  scratch.axis_y.x = 0.0;
  scratch.axis_y.y = 1.0;
  scratch.axis_y.z = 0.0;
  scratch.axis_z.x = 0.0;
  scratch.axis_z.y = 0.0;
  scratch.axis_z.z = 1.0;
  scratch.colour_a_r = 0xff;
  scratch.colour_a_g = 0;
  scratch.colour_a_b = 0;
  scratch.colour_a_a = 0xff;
  scratch.colour_b_r = 0;
  scratch.colour_b_g = 0;
  scratch.colour_b_b = 0xff;
  scratch.colour_b_a = 0xff;
  g_weapon_6a6230 = 0;
  cursor = (int *)&g_weapon_machinegun_6a6234;
  /*
   * cursor points at the field_0x44 word of the entry above the one being updated,
   * so the entry is cursor - 0x23 ints (its slot, +0x00). Offsets used below:
   *   cursor[-0x23] slot +0x00        cursor[-0x22] cooldown +0x04
   *   cursor[-0x21] clump +0x08       cursor[-0x20] active +0x0c
   *   cursor[-0x1d] spin +0x18        cursor[-0x1c] barrel roll angle +0x1c (inferred)
   *   cursor[-0x1b] elevation angle +0x20 (inferred)   cursor[-0x1a] yaw angle +0x24 (inferred)
   *   cursor[-0x18] +0x2c, cursor[-0x17] +0x30: gun-part objects (inferred)
   *   cursor[-0x15] +0x38 yaw mount frame, cursor[-0x14] +0x3c barrel frame, cursor[-0x13] +0x40 elevation mount frame
   *   *next_cursor +0x44: base object (its parent frame is the cone origin)
   */
  do {
    next_cursor = cursor + -MACHINEGUN_ENTRY_INTS;
    gun_index = gun_index + -1;
    object_set_flag_4(cursor[-0x17],0);
    object_set_flag_4(cursor[-0x18],0);
    object_set_flag_4(*next_cursor,0);
    /* Only an active gun updates */
    if (cursor[-0x20] != 0) {
      RwFrameTransform(*(RwFrame **)(cursor[-0x21] + 4),(RwMatrix *)cursor[-0x23],rwCOMBINEREPLACE);
      slot_value = weapon_effect_get_slot_value_b(gun_index);
      /* Cone colour: colour A, or colour B when slot value a differs from the fx rwp id (inferred) */
      if (slot_value == SLOT_VALUE_NONE) {
        slot_value = weapon_effect_get_slot_value_a(gun_index);
        rwp_id = fx_object_get_rwp_id(0);
        cone_colour = &scratch.colour_a_r;
        if (slot_value != rwp_id) {
          cone_colour = &scratch.colour_b_r;
        }
      }
      else {
        cone_colour = &scratch.colour_a_r;
      }
      /* Cone size: 0x41000000 = 8.0f (length), 0x41800000 = 16.0f (radius) (inferred) */
      cone_radius = 0x41800000;
      cone_length = 0x41000000;
      ltm = RwFrameGetLTM(*(RwFrame **)(*next_cursor + 4));
      weapon_find_targets_in_cone(gun_index,&ltm->pos,cone_length,cone_radius,cone_colour);
      scratch.pitch_step = 0.0;
      scratch.yaw_rate_dead = 0.0;
      ltm = RwFrameGetLTM(*(RwFrame **)(cursor[-0x13] + 4));
      RwMatrixInvert(&scratch.gun_inverse_ltm,ltm);
      /* Aim point, then into the elevation mount's local space */
      aim_record = (RwReal *)weapon_effect_get_slot_record(*(undefined4 *)(cursor[-0x23] + 0xb0));
      scratch.aim_point.x = *aim_record;
      scratch.aim_point.y = aim_record[1];
      scratch.aim_point.z = aim_record[2];
      RwV3dTransformPoints(&scratch.aim_local,&scratch.aim_point,1,&scratch.gun_inverse_ltm);
      /*
       * Aim rates: x error * 2.5 gives the yaw rate, y error * -2.5 gives the pitch rate
       * (constants from the extern names, inferred). Errors inside the dead zone give 0;
       * each rate is clamped to +/-3.
       */
      if ((k_f_0_1 < scratch.aim_local.x) || (tmp_f = scratch.yaw_rate_dead, scratch.aim_local.x < k_f_n0_1)) {
        tmp_f = scratch.aim_local.x * k_f_2_5;
      }
      if ((k_f_0_1 < scratch.aim_local.y) || (scratch.aim_local.y < k_f_n0_1)) {
        scratch.pitch_step = scratch.aim_local.y * k_f_n2_5;
        if (scratch.pitch_step > k_f_3) {
          scratch.pitch_step = 3.0;
        }
        else {
          if (scratch.pitch_step < k_f_n3) {
            scratch.pitch_step = -3.0;
          }
        }
      }
      if (tmp_f > k_f_3) {
        scratch.yaw_step = k_f_3;
      }
      else {
        scratch.yaw_step = tmp_f;
        if (tmp_f < k_f_n3) {
          scratch.yaw_step = k_f_n3;
        }
      }
      /* Add the rates to the mount angles, wrapped into [0, 360) */
      scratch.pitch_step = scratch.pitch_step + *(float *)&cursor[-0x1b];
      *(float *)&cursor[-0x1b] = scratch.pitch_step;
      if (k_f_360 < scratch.pitch_step) {
        *(float *)&cursor[-0x1b] = (scratch.pitch_step - k_f_360);
      }
      if (*(float *)&cursor[-0x1b] < k_f_0) {
        *(float *)&cursor[-0x1b] = (*(float *)&cursor[-0x1b] + k_f_360);
      }
      scratch.yaw_step = scratch.yaw_step + *(float *)&cursor[-0x1a];
      *(float *)&cursor[-0x1a] = scratch.yaw_step;
      if (k_f_360 < scratch.yaw_step) {
        *(float *)&cursor[-0x1a] = (scratch.yaw_step - k_f_360);
      }
      if (*(float *)&cursor[-0x1a] < k_f_0) {
        *(float *)&cursor[-0x1a] = (*(float *)&cursor[-0x1a] + k_f_360);
      }
      /* Apply: yaw about Y, elevation about X, barrel roll about Z */
      RwFrameRotate(*(RwFrame **)(cursor[-0x15] + 4),&scratch.axis_y,*(float *)&cursor[-0x1a],
                    rwCOMBINEREPLACE);
      RwFrameRotate(*(RwFrame **)(cursor[-0x13] + 4),&scratch.axis_x,*(float *)&cursor[-0x1b],
                    rwCOMBINEREPLACE);
      RwFrameRotate(*(RwFrame **)(cursor[-0x14] + 4),&scratch.axis_z,*(float *)&cursor[-0x1c],
                    rwCOMBINEREPLACE);
      /* Spin decays by 1.0 per call (floored at 0) and feeds the barrel roll angle; tmp_f is reused here */
      tmp_f = *(float *)&cursor[-0x1d] - float_1_0;
      *(float *)&cursor[-0x1d] = tmp_f;
      if (tmp_f < k_f_0) {
        cursor[-0x1d] = 0;
      }
      *(float *)&cursor[-0x1c] = (*(float *)&cursor[-0x1d] + *(float *)&cursor[-0x1c]);
      /* Cooldown counts down by g_tick_seconds, floored at 0 */
      if (*(float *)&cursor[-0x22] > k_f_0) {
        *(float *)&cursor[-0x22] = (*(float *)&cursor[-0x22] - g_tick_seconds);
      }
      else {
        cursor[-0x22] = 0;
      }
    }
    cursor = next_cursor;
  } while (next_cursor != (int *)&MachinegunState_ARRAY_006a60d0[0].field_0x44);
  particles_flush(&g_weapon_6a61f0);
  return;
}


