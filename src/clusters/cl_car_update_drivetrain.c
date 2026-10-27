// cluster-entries: car_update_drivetrain
/* Installed cluster car_update_drivetrain (tools/cluster_install.py from output/cluster_pilot_j1/car_update_drivetrain.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/* DECLS-BEGIN */
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004b2b48*/ extern /* RET? */ undefined _ftol();
/*0042b110*/ extern int current_course_id(void);
/*008bd7a8*/ extern undefined4 g_cars_8bd7a8;
/*008bd7ac*/ extern undefined4 g_cars_8bd7ac;
/*0040fdb0*/ extern int get_player_count(void);
/*005ddc28*/ extern float k_f_0;
/*005de208*/ extern float k_f_0_001953125;
/*005de8c4*/ extern float k_f_0_016666668;
/*005dfd3c*/ extern float k_f_0_019904457;
/*005de490*/ extern float k_f_0_02;
/*005dd628*/ extern float k_f_0_25;
/*005ddb20*/ extern float k_f_0_9;
/*005dfd20*/ extern float k_f_10000000;
/*005dfd34*/ extern float k_f_1024;
/*005de22c*/ extern float k_f_1500;
/*005dfd58*/ extern float k_f_150000;
/*005ddb48*/ extern float k_f_16;
/*005dfd60*/ extern float k_f_160;
/*005de19c*/ extern float k_f_1_0en4;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005dfd54*/ extern float k_f_200000;
/*005df2e0*/ extern float k_f_250;
/*005dfd50*/ extern float k_f_300000;
/*005dfd24*/ extern float k_f_32768;
/*005dfd5c*/ extern float k_f_350000;
/*005de244*/ extern float k_f_500;
/*005ddb50*/ extern float k_f_8;
/*005dfd4c*/ extern float k_f_8000000;
/*005dfd40*/ extern float k_f_n1_0en5;
/*0040fdc0*/ extern int race_get_phase(void);
extern float float_1_0, float_10_0, float_50_0, k_f_1_7453333, k_f_256, g_car_8bd730, k_f_0_008, k_f_0_01, k_f_0_1, k_f_0_5, k_f_0_85, k_f_0_99, k_f_1_0en7, k_f_1_1, k_f_1_25, k_f_1_5, k_f_3, k_f_360, k_f_3_0517578en5, k_f_3_3333333en4, k_f_50_24, k_f_6_6666666en4, k_f_9_259333en7, k_f_9_765625en4;
/* DECLS-END */
/* CFLD(car, 0xb14/0xb18/0xb1c) is Car.force_accum[0..2], CFLDI(car, 0xb20) is Car.driven; the typed members compile differently in this function (not object identical) */
#define CFLD(c, off) (*(float *)((char *)(c) + (off)))
#define CFLDI(c, off) (*(int *)((char *)(c) + (off)))
#define PF(i) (*(float *)(wheel + (i)))
/* Drivetrain of one car for one frame (inferred names): gear selection from the speed, engine / turbo force on the wheels in contact,
   sideways slip per wheel, speed and angular speed updates.  The x87 temporaries that the original keeps unrounded are doubles, the
   ones it stores to the stack are volatile floats. */
/* Multi-purpose locals (kept as one variable each: splitting them per use changes the register allocation of the object): scratch_i = race phase (until the throttle test), then the surface colour (0xaarrggbb) compared against the road/dirt tables, later the turbo charge / wheel index / contact flag;
   scratch_f3 = torque limit of the gear table (1500 - field_b0c, min 500), then the throttle input 0..160, then the boost / impulse scale; scratch_f2 = gear_torque * ratio (the force term of the slip test); scratch_f4 = torque base, then speed term (speed * 0.01 + 10). */
void car_update_drivetrain_n(Car *car,int car_slot,float dt,undefined4 param_4,int controls)

{
  RwV3d *in;
  float *vel_ptr;
  float scratch_f2;
  float gear_demand;
  float scratch_f3;
  float scratch_f4;
  float fVar5;
  float slip_threshold;
  double drag_damp;
  bool boost_active;
  int scratch_i;
  int course_id;
  int gear_probe;
  int wheel_index;
  int gear_max;
  int *wheel;
  double dot;
  double dt_scaled, slip_energy_add, forward_align, slide_amount, dotv, grip_factor, px, py, drive_force, b2e;
  volatile float projy, projz, vq2, vq3, vq4;
  double ratio, zacc, rdiff, tqs, rlen;
  volatile float v8c, rl_f;
  double sc, u, gx, tz, coef, vv, zt, zv;
  volatile float ty, tz2, xv, yv;
  float s2x, s2y, s2z;
  double inv;
  volatile float tq_y, tq_z, pz;       /* the original stores both products to stack temps (rounded to float) before the subtractions */
  RwReal vec_len;
  float traction;
  RwV3d tmp_vec;
  float gear_torque;
  float slip_energy;
  float side_grip;
  float slip_x;
  float slip_y;
  float slip_z;
  RwV3d side_dir;
  float force_x;
  float force_y;
  float force_z;
  float drag_sum;
  float tmp_9c;
  float tmp_98;
  float tmp_94;
  uint wheel_flags;
  float dir_x;
  float dir_y;
  float dir_z;
  RwV3d torque_dir;
  RwV3d torque_sum;
  float contact_sum_x;
  float contact_sum_y;
  float contact_sum_z;
  int race_phase;
  int spinning;
  float gear_torque_tab [5];
  RwMatrix rot_matrix;
  
  scratch_i = race_get_phase();
  race_phase = scratch_i;
  vec_len = RwV3dLength((RwV3d *)car->velocity);
  car->speed = vec_len;
  vec_len = RwV3dLength((RwV3d *)car->angular_velocity);
  car->angular_speed = vec_len;
  slip_energy = 0.0;
  torque_sum.z = 0.0;
  torque_sum.y = 0.0;
  torque_sum.x = 0.0;
  contact_sum_z = 0.0;
  scratch_f3 = k_f_1500 - *(float *)&car->field_b0c;
  contact_sum_y = 0.0;
  contact_sum_x = 0.0;
  force_z = 0.0;
  force_y = 0.0;
  force_x = 0.0;
  if (scratch_f3 <= k_f_500) {
    scratch_f3 = k_f_500;
  }
  wheel_index = 0;
  gear_max = 0;
  gear_probe = 0;
  scratch_f4 = car->handling_c4 * k_f_0_5 * scratch_f3 * k_f_6_6666666en4;
  scratch_f3 = scratch_f3 * k_f_6_6666666en4 * car->handling_c0 * k_f_0_5;
  do {
    if (car->handling_params[gear_probe] != k_f_0) {
      if (gear_probe == 0) {
        gear_demand = (car->speed + k_f_250) * car->handling_params[0];
      }
      else {
        gear_demand = car->handling_params[gear_probe] * car->speed;
      }
      if ((scratch_f3 < gear_demand) && (gear_demand = scratch_f3, gear_probe != 4)) {
        gear_max = gear_max + 1;
      }
      gear_torque_tab[gear_probe] = ((scratch_f4 / car->handling_params[gear_probe]) / scratch_f3) * gear_demand + car->speed
      ;
    }
    if (car->handling_params[gear_probe + 1] != k_f_0) {
      if (gear_probe == -1) {
        gear_demand = (car->speed + k_f_250) * car->handling_params[0];
      }
      else {
        gear_demand = car->handling_params[gear_probe + 1] * car->speed;
      }
      if ((scratch_f3 < gear_demand) && (gear_demand = scratch_f3, gear_probe != 3)) {
        gear_max = gear_max + 1;
      }
      gear_torque_tab[gear_probe + 1] =
           ((scratch_f4 / car->handling_params[gear_probe + 1]) / scratch_f3) * gear_demand + car->speed;
    }
    if (car->handling_params[gear_probe + 2] != k_f_0) {
      if (gear_probe == -2) {
        gear_demand = (car->speed + k_f_250) * car->handling_params[0];
      }
      else {
        gear_demand = car->handling_params[gear_probe + 2] * car->speed;
      }
      if ((scratch_f3 < gear_demand) && (gear_demand = scratch_f3, gear_probe != 2)) {
        gear_max = gear_max + 1;
      }
      gear_torque_tab[gear_probe + 2] =
           ((scratch_f4 / car->handling_params[gear_probe + 2]) / scratch_f3) * gear_demand + car->speed;
    }
    if (car->handling_params[gear_probe + 3] != k_f_0) {
      gear_demand = car->speed;
      if (gear_probe == -3) {
        gear_demand = gear_demand + k_f_250;
      }
      gear_demand = gear_demand * car->handling_params[gear_probe + 3];
      if ((scratch_f3 < gear_demand) && (gear_demand = scratch_f3, gear_probe != 1)) {
        gear_max = gear_max + 1;
      }
      gear_torque_tab[gear_probe + 3] =
           ((scratch_f4 / car->handling_params[gear_probe + 3]) / scratch_f3) * gear_demand + car->speed;
    }
    if (car->handling_params[gear_probe + 4] != k_f_0) {
      if (gear_probe == -4) {
        gear_demand = (car->speed + k_f_250) * car->handling_params[0];
      }
      else {
        gear_demand = car->handling_params[gear_probe + 4] * car->speed;
      }
      if ((scratch_f3 < gear_demand) && (gear_demand = scratch_f3, gear_probe != 0)) {
        gear_max = gear_max + 1;
      }
      gear_torque_tab[gear_probe + 4] =
           ((scratch_f4 / car->handling_params[gear_probe + 4]) / scratch_f3) * gear_demand + car->speed;
    }
    gear_probe = gear_probe + 5;
  } while (gear_probe < 5);
  gear_probe = car->gear;
  if (gear_max != gear_probe) {
    if ((car->gear_shift_timer == 0) && (gear_probe < gear_max)) {
      car->gear_shift_timer = 3000;
      car->gear = gear_probe + 1;
    }
    if (gear_max < car->gear) {
      car->gear_shift_timer = -3000;
      car->gear = gear_max;
    }
  }
  gear_probe = car->gear;
  gear_torque = gear_torque_tab[gear_probe];
  if (0 < car->gear_shift_timer) {
    gear_max = (int)((float)car->gear_shift_timer - dt);
    car->gear_shift_timer = gear_max;
    if (gear_max < 0) {
      car->gear_shift_timer = 0;
    }
  }
  if (car->gear_shift_timer < 0) {
    gear_max = (int)((float)car->gear_shift_timer + dt);
    car->gear_shift_timer = gear_max;
    if (0 < gear_max) {
      car->gear_shift_timer = 0;
    }
  }
  spinning = 0;
  scratch_f3 = k_f_0;
  if ((scratch_i != 7) &&
     (scratch_f3 = (float)*(byte *)(controls + 4), k_f_160 < (float)*(byte *)(controls + 4))) {
    scratch_f3 = k_f_160;
  }
  scratch_f2 = gear_torque * car->handling_params[gear_probe];
  scratch_i = car->wheels[0].surface_color;
  scratch_f4 = car->speed * k_f_0_01 + float_10_0;
  slip_threshold = k_f_150000;
  if ((scratch_i != -0x5f7f80) && (slip_threshold = k_f_200000, scratch_i != -0x557f80)) {
    slip_threshold = k_f_350000;
    if (((scratch_i == -0x69e1a6) || (scratch_i == -0xe17f4c)) &&
       ((car->is_bot == 0 &&
        (course_id = current_course_id(), slip_threshold = k_f_300000, course_id == 20)))) {
      slip_threshold = k_f_150000;
    }
    if ((scratch_i == -1) || (scratch_i == -0x373738)) {
      { double dragt = dt * k_f_0_016666668;      /* keeps the original product order */
        drag_damp = (float_1_0 - dragt * k_f_0_01) * k_f_0_99; }
      car->velocity[0] = drag_damp * car->velocity[0];
      car->velocity[1] = drag_damp * car->velocity[1];
      car->velocity[2] = drag_damp * car->velocity[2];
    }
  }
  if (slip_threshold < (scratch_f2 / scratch_f4) * scratch_f3) {
    spinning = 1;
    gear_torque = gear_torque * k_f_0_5;
  }
  drag_sum = 0.0;
  wheel = &car->wheels[0].spin_rate;
  do {
    boost_active = false;
    if ((((Wheel *)(wheel + -0xf))->state == 2) && (wheel[-3] != 0)) {
      traction = (float)*(byte *)(controls + 4);
      if (race_phase == 7) {
        traction = 0.0;
      }
      if (car->effect_level[2] != 0) {
        traction = (float)(int)(100 - car->effect_level[2]) * traction * k_f_0_01;
      }
      scratch_i = car->wheels[0].surface_color;
      if (((scratch_i == -0x69e1a6) || (scratch_i == -0xe17f4c)) && (k_f_0 < car->speed)) {
        RwV3dNormalize(&tmp_vec,(RwV3d *)car->velocity);
        forward_align = (car->forward[1] * tmp_vec.y + car->forward[0] * tmp_vec.x) + car->forward[2] * tmp_vec.z;
        forward_align = forward_align * forward_align;
        forward_align = forward_align * forward_align;
        forward_align = forward_align * forward_align;
        if (forward_align < k_f_0) {
          forward_align = -forward_align;
        }
        if (forward_align < k_f_0_25) {
          forward_align = k_f_0_25;
        }
        if ((car->is_bot == 0) && (forward_align < k_f_0_5)) {
          forward_align = k_f_0_5;
        }
        traction = forward_align * traction;
      }
      scratch_f3 = k_f_160;
      if ((race_phase != 7) && (traction != k_f_0)) {
        boost_active = true;
        CFLDI(car, 0xb20) = 1;
        if (scratch_f3 < traction) {
          traction = 160.0;
        }
        drive_force = traction * gear_torque;
        CFLD(car, 0xb14) = PF(0x1f) * drive_force + CFLD(car, 0xb14);
        CFLD(car, 0xb18) = PF(0x20) * drive_force + CFLD(car, 0xb18);
        CFLD(car, 0xb1c) = PF(0x21) * drive_force + CFLD(car, 0xb1c);
      }
      scratch_i = race_get_phase();
      if (scratch_i == 6) {
        scratch_i = (int)((float)car->turbo_charge - dt);
        car->turbo_charge = scratch_i;
        if (3000 < scratch_i) {
          car->turbo_charge = 3000;
        }
        if (car->turbo_charge < 0) {
          car->turbo_charge = 0;
        }
      }
      if (car->turbo_state == 1) {
        if (car->turbo_charge == 0) {
          car->turbo_state = 0;
        }
        else {
          scratch_i = get_player_count();
          if ((scratch_i != 4) ||
             ((scratch_f3 = k_f_8000000, g_cars_8bd7a8 != car_slot && (g_cars_8bd7ac != car_slot)))) {
            scratch_f3 = 5e+06;
          }
          CFLD(car, 0xb14) = PF(0x1f) * scratch_f3 + CFLD(car, 0xb14);
          CFLD(car, 0xb18) = PF(0x20) * scratch_f3 + CFLD(car, 0xb18);
          CFLD(car, 0xb1c) = PF(0x21) * scratch_f3 + CFLD(car, 0xb1c);
          scratch_i = (int)((float)car->turbo_charge - dt);
          car->turbo_charge = scratch_i;
          if (scratch_i < 1) {
            car->turbo_state = 0;
            car->turbo_charge = 0;
          }
        }
      }
      if (car->turbo_state == 2) {
        if (car->turbo_charge == 0) {
          car->turbo_state = 0;
        }
        else {
          CFLD(car, 0xb1c) = 0;
          CFLD(car, 0xb18) = 0;
          CFLD(car, 0xb14) = 0;
          scratch_i = (int)((float)car->turbo_charge - dt);
          car->turbo_charge = scratch_i;
          if (scratch_i < 1) {
            car->turbo_state = 0;
            car->turbo_charge = 0;
          }
        }
      }
    }
    scratch_i = (int)(((PF(0x1f) * car->velocity[0] + PF(0x20) * car->velocity[1]) + PF(0x21) * car->velocity[2]) / (PF(-10) * k_f_1_7453333));
    *wheel = scratch_i;
    if ((spinning == 0) || (((Wheel *)(wheel + -0xf))->state != 2)) {
      wheel[-1] = 0;
    }
    else {
      scratch_i = wheel[-1];
      wheel[-1] = scratch_i + 1;
      if (scratch_i + 1 < 0x200) {
        wheel[-1] = 0x200;
      }
      *wheel = *wheel + (wheel[-1] + -0x200) * 0x40;
    }
    if (wheel[-3] != 0) {
      if ((*(char *)(controls + 5) != '\0') && (*(char *)(controls + 4) == '\0')) {
        CFLDI(car, 0xb20) = 1;
        if (*wheel < 1) {
          if (((Wheel *)(wheel + -0xf))->state == 2) {
            scratch_f3 = (float)*(byte *)(controls + 5);
            if (car->effect_level[2] != 0) {
              scratch_f3 = scratch_f3 * (float)(int)(100 - car->effect_level[2]) * k_f_0_01;
            }
            if (k_f_160 < scratch_f3) {
              scratch_f3 = k_f_160;
            }
            scratch_f3 = -(gear_torque * k_f_0_9 * scratch_f3);
            PF(0x1c) = scratch_f3 * PF(0x1f) + PF(0x1c);
            PF(0x1d) = scratch_f3 * PF(0x20) + PF(0x1d);
            PF(0x1e) = scratch_f3 * PF(0x21) + PF(0x1e);
          }
        }
        else if (1 < wheel_index) {
          scratch_i = (int)((float)*(byte *)(controls + 5) + k_f_256);
          wheel[-1] = scratch_i;
        }
      }
      if (((((car->angular_velocity[0] < k_f_n1_0en5) || (k_f_1_0en5 < car->angular_velocity[0])) ||
           (car->angular_velocity[1] < k_f_n1_0en5)) ||
          ((k_f_1_0en5 < car->angular_velocity[1] || (car->angular_velocity[2] < k_f_n1_0en5)))) ||
         (k_f_1_0en5 < car->angular_velocity[2])) {
        RwMatrixRotate(&rot_matrix,(RwV3d *)car->angular_velocity,270.0,rwCOMBINEREPLACE);
        RwV3dTransformVectors(&tmp_vec,(RwV3d *)(wheel + -9),1,&rot_matrix);
        sc = car->angular_speed * k_f_0_019904457;
        u = car->speed * k_f_0_008;
        if (u < k_f_8) {
          u = k_f_8;
        }
        sc = (sc * u) * k_f_360;
        gx = tmp_vec.x * sc;
        ty = tmp_vec.y * sc;
        tz2 = tmp_vec.z * sc;
        tmp_vec.x = gx - car->velocity[0];
        tmp_vec.y = ty - car->velocity[1];
        tmp_vec.z = tz2 - car->velocity[2];
        traction = RwV3dLength(&tmp_vec);
      }
      else {
        traction = car->speed;
        tmp_vec.x = -car->velocity[0];
        tmp_vec.y = -car->velocity[1];
        tmp_vec.z = -car->velocity[2];
      }
      if ((k_f_1_0en4 < traction) && (*(int *)&car->wheels_in_contact == 0x40800000)) {
        inv = float_1_0 / traction;
        gear_torque_tab[0] = tmp_vec.x * inv;
        gear_torque_tab[1] = tmp_vec.y * inv;
        tz = inv * tmp_vec.z;
        coef = PF(0x15) * PF(0x1b) * g_car_8bd730;
        if (k_f_1024 < traction) {
          traction = 1024.0f;
        }
        side_grip = (coef * traction) * k_f_9_765625en4;
        s2x = 0.0f;
        s2y = 0.0f;
        s2z = 0.0f;
        dot = (car->forward[1] * gear_torque_tab[1] + car->forward[0] * gear_torque_tab[0]) + car->forward[2] * tz;
        slip_x = dot * PF(0x1f);
        slip_y = dot * PF(0x20);
        slip_z = dot * PF(0x21);
        side_dir.x = gear_torque_tab[0] - slip_x;
        side_dir.y = gear_torque_tab[1] - slip_y;
        side_dir.z = tz - slip_z;
        rlen = RwV3dLength(&side_dir);      /* fst: stored as a float AND the unrounded ST0 is multiplied */
        rl_f = (float)rlen;
        vec_len = rl_f;
        wheel_flags = wheel[-1];
        drag_sum = rlen * traction + drag_sum;
        if ((wheel_flags & 0x100) == 0) {
          if (wheel_flags == 0) {
            coef = PF(0x16) * PF(0x1b) * g_car_8bd730;
            s2x = slip_x * coef;
            s2y = slip_y * coef;
            s2z = coef * slip_z;
            vv = vec_len * car->speed;
            if (vv < float_50_0) {
              vv = (vv * k_f_0_02) * side_grip;
            }
            else {
              vv = side_grip;
            }
          }
          else {
            vv = side_grip;
          }
          s2x = side_dir.x * vv + s2x;
          s2y = side_dir.y * vv + s2y;
          zt = vv * side_dir.z + s2z;
        }
        else {
          vv = side_grip * k_f_0_85;
          xv = side_dir.x * vv;
          yv = side_dir.y * vv;
          zv = vv * side_dir.z;
          coef = PF(0x1b) * k_f_3 * g_car_8bd730 * (float)(int)wheel_flags * k_f_0_001953125 * k_f_1_1;
          if (boost_active) {
            coef = coef * k_f_0_5;
          }
          s2x = slip_x * coef + xv;
          s2y = slip_y * coef + yv;
          zt = slip_z * coef + zv;
        }
        PF(0x1c) = s2x + PF(0x1c);
        PF(0x1d) = s2y + PF(0x1d);
        PF(0x1e) = zt + PF(0x1e);
      }
    }
    in = (RwV3d *)(wheel + 0x1c);
    vec_len = RwV3dLength(in);
    if (k_f_1_0en4 < vec_len) {
      inv = float_1_0 / PF(-0xb);
      dir_x = PF(-9) * inv;
      dir_y = PF(-8) * inv;
      dir_z = PF(-7) * inv;
      dot = (dir_y * PF(0x1d) + dir_x * in->x) + dir_z * PF(0x1e);
      slip_x = dir_x * dot;
      slip_y = dir_y * dot;
      slip_z = dir_z * dot;
      side_dir.x = in->x - slip_x;
      side_dir.y = PF(0x1d) - slip_y;
      b2e = PF(0x1e) - slip_z;
      side_dir.z = (float)b2e;
      torque_dir.x = b2e * slip_y - side_dir.y * slip_z;
      torque_dir.y = side_dir.x * slip_z - side_dir.z * slip_x;
      torque_dir.z = side_dir.y * slip_x - side_dir.x * slip_y;
      if (k_f_0 < dot) {
        torque_dir.x = -torque_dir.x;
        torque_dir.y = -torque_dir.y;
        torque_dir.z = -torque_dir.z;
      }
      force_x = slip_x + force_x;
      force_y = slip_y + force_y;
      force_z = slip_z + force_z;
      contact_sum_x = contact_sum_x + in->x;
      contact_sum_y = contact_sum_y + PF(0x1d);
      contact_sum_z = contact_sum_z + PF(0x1e);
      tmp_vec.x = side_dir.x * PF(-0xb);
      tmp_vec.y = side_dir.y * PF(-0xb);
      tmp_vec.z = side_dir.z * PF(-0xb);
      vec_len = RwV3dLength(&tmp_vec);
      slip_energy_add = vec_len * k_f_50_24;
      { volatile float vf = (float)slip_energy_add; scratch_f3 = vf; }      /* stored to a stack float in the original (fst) */
      slip_energy = slip_energy_add + slip_energy;
      vec_len = RwV3dLength(&torque_dir);
      if (k_f_1_0en4 < vec_len) {
        double q = scratch_f3 / vec_len;      /* stays on the x87 stack (unrounded) in the original */
        tq_y = torque_dir.y * q;
        tq_z = torque_dir.z * q;
        torque_sum.x = torque_sum.x - torque_dir.x * q;
        torque_sum.y = torque_sum.y - tq_y;
        torque_sum.z = torque_sum.z - tq_z;
      }
    }
    wheel_index = wheel_index + 1;
    wheel = wheel + 0x31;
  } while (wheel_index < 4);
  tqs = dt * car->inertia_inverse * k_f_9_259333en7;      /* never stored in the original (x87 register) */
  tmp_vec.x = torque_sum.x * tqs;
  tmp_vec.y = torque_sum.y * tqs;
  tmp_vec.z = tqs * torque_sum.z;
  if (car->controls_disabled == 0) {
    car->angular_velocity[0] = tmp_vec.x + car->angular_velocity[0];
    car->angular_velocity[1] = tmp_vec.y + car->angular_velocity[1];
    car->angular_velocity[2] = tmp_vec.z + car->angular_velocity[2];
  }
  vec_len = RwV3dLength(&torque_sum);
  zacc = force_z;
  rdiff = slip_energy - vec_len;      /* double: the original keeps the difference on the x87 stack (cl would spill a float temporary) */
  if (rdiff != k_f_0) {
    ratio = rdiff / slip_energy;
    tmp_vec.x = contact_sum_x - force_x;
    tmp_vec.y = contact_sum_y - force_y;
    tmp_vec.z = contact_sum_z - force_z;
    force_x = tmp_vec.x * ratio + force_x;
    force_y = tmp_vec.y * ratio + force_y;
    zacc = ratio * tmp_vec.z + force_z;
  }
  vel_ptr = car->velocity;
  { volatile float t_a4 = CFLD(car, 0xb1c) + zacc; force_z = t_a4; }      /* stored as a float in the original */
  dt_scaled = dt * car->time_step_const * k_f_3_3333333en4;
  slip_energy = (float)dt_scaled;
  v8c = (float)(dt_scaled * (CFLD(car, 0xb14) + force_x));      /* stored as a float in the original */
  *vel_ptr = v8c + *vel_ptr;
  car->velocity[1] = slip_energy * (CFLD(car, 0xb18) + force_y) + car->velocity[1];
  car->velocity[2] = slip_energy * force_z + car->velocity[2];
  vec_len = RwV3dLength((RwV3d *)vel_ptr);
  car->speed = vec_len;
  scratch_i = car->wheels[0].surface_color;
  slide_amount = drag_sum / car->wheels[0].handling_scale;
  drag_sum = (float)slide_amount;
  if (scratch_i == (int)0xffa08080) {
    slide_amount = slide_amount * k_f_1_5;
  }
  else if (scratch_i == (int)0xffaa8080) {
    slide_amount = slide_amount + slide_amount;
  }
  else {
    if (scratch_i == (int)0xff961e5a || scratch_i == (int)0xff1e80b4) {
      if (car->is_bot == 0) {
        if (current_course_id() == 20) {
          slide_amount = drag_sum * k_f_1_5;
        }
        else {
          slide_amount = drag_sum * k_f_3;
        }
      }
      else {
        slide_amount = slide_amount * k_f_1_5;
      }
    }
    if (scratch_i == (int)0xffc81e5a) {
      if (car->is_bot == 0) {
        slide_amount = slide_amount + slide_amount;
      }
      else {
        slide_amount = slide_amount * k_f_1_25;
      }
    }
  }
  if (car->effect_level[3] != 0) {
    slide_amount = ((float)(int)car->effect_level[3] * k_f_0_01 + float_1_0) * slide_amount;
  }
  if (car->effect_level[5] != 0) {
    slide_amount = ((float)(int)car->effect_level[5] * k_f_0_1 + float_1_0) * slide_amount;
  }
  if ((car->speed != k_f_0) && (*(int *)&car->wheels_in_contact == 0x40800000)) {
    dotv = (car->forward[0] * car->velocity[0] + car->forward[1] * car->velocity[1]) + car->velocity[2] * car->forward[2];
    vq4 = car->velocity[0] - dotv * car->forward[0];
    projy = dotv * car->forward[1];
    projz = dotv * car->forward[2];
    vq2 = car->velocity[1] - projy;
    vq3 = car->velocity[2] - projz;
    slide_amount = slide_amount * car->speed;
    if (k_f_32768 < slide_amount) {
      grip_factor = (k_f_10000000 - slide_amount) * k_f_1_0en7;
      if (grip_factor < k_f_0) {
        grip_factor = k_f_0;
      }
      grip_factor = grip_factor * k_f_0_1 + grip_factor * k_f_0_1;
      px = vq4 * grip_factor;
      py = vq2 * grip_factor;
      pz = vq3 * grip_factor;
      car->velocity[0] = car->velocity[0] - px;
      car->velocity[1] = car->velocity[1] - py;
      car->velocity[2] = car->velocity[2] - pz;
      grip_factor = float_1_0 - grip_factor;
      car->angular_velocity[0] = grip_factor * car->angular_velocity[0];
      car->angular_velocity[1] = grip_factor * car->angular_velocity[1];
      car->angular_velocity[2] = grip_factor * car->angular_velocity[2];
      return;
    }
    grip_factor = (k_f_32768 - slide_amount) * k_f_3_0517578en5;
    if (grip_factor < k_f_0_1) {
      grip_factor = k_f_0_1;
    }
    px = vq4 * grip_factor;
    py = vq2 * grip_factor;
    pz = vq3 * grip_factor;
    car->velocity[0] = car->velocity[0] - px;
    car->velocity[1] = car->velocity[1] - py;
    car->velocity[2] = car->velocity[2] - pz;
    if (grip_factor < k_f_0_5) {
      grip_factor = k_f_0_5;
    }
    grip_factor = float_1_0 - grip_factor;
    scratch_i = CFLDI(car, 0xb20);
    car->angular_velocity[0] = grip_factor * car->angular_velocity[0];
    car->angular_velocity[1] = grip_factor * car->angular_velocity[1];
    car->angular_velocity[2] = grip_factor * car->angular_velocity[2];
    if (scratch_i == 0 && car->speed < k_f_16) {
      car->velocity[0] = 0.0f;
      car->velocity[2] = 0.0f;
      car->velocity[1] = 0.0f;
      car->angular_velocity[2] = 0.0f;
      car->angular_velocity[1] = 0.0f;
      car->angular_velocity[0] = 0.0f;
      return;
    }
  }
}

/* entry thunk at the original address of car_update_drivetrain: register inputs (esi,s0,s1,s2,s3) -> natural C car_update_drivetrain_n; ret 0 */
__declspec(naked) void car_update_drivetrain(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push esi
        call car_update_drivetrain_n
        add esp, 20
        ret
    }
}
