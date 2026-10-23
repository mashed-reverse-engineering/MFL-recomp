// cluster-entries: car_ground_contact_forces
/* Installed cluster car_ground_contact_forces (tools/cluster_install.py from output/cluster_pilot_h3/car_ground_contact_forces.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd378*/ extern float k_f_0_1;
/*005dd628*/ extern float k_f_0_25;
/*005dda24*/ extern float k_f_0_75;
/*005ddad4*/ extern float k_f_90;
/*005ddb20*/ extern float k_f_0_9;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de244*/ extern float k_f_500;
/*005de290*/ extern float float_50_0;
/*005de6a8*/ extern float k_f_n0_5;
/*005dfd2c*/ extern float k_f_50_24;
/*005dfda0*/ extern float k_f_0_0027778;
/* car_ground_contact_forces */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "counts wheels in contact (+0x198 / +0x25C / +0x320 / +0x3E4) and builds the contact forces
   (inferred)" */

void car_ground_contact_forces_n(Car *car)

{
  float tmp_f;
  float vel_y_new;
  float vel_z_new;
  float damp;
  float *contact_next;
  int contact_index;
  float *contact;
  int wheels_touching;
  RwReal vec_len;
  struct {
    float pad_bot;
    float tangent_force_sum;
    float grip_factor;
    undefined1 pad_70[4];
    RwV3d torque_sum;
    float total_force_x;
    float total_force_y;
    float total_force_z;
    RwV3d net_force;
    RwV3d extra_force;
    RwV3d contact_force;
    RwV3d torque_axis;
    RwV3d normal_dir;
    float tangent_x;
    float tangent_y;
    float tangent_z;
    float normal_part_x;
    float normal_part_y;
  } frame;
  
  contact_index = 0;
  /* count wheels touching the ground */
  wheels_touching = car->wheels[0].in_contact != 0;
  frame.tangent_force_sum = 0.0;
  frame.torque_sum.z = 0.0;
  frame.torque_sum.y = 0.0;
  frame.torque_sum.x = 0.0;
  frame.total_force_z = 0.0;
  frame.total_force_y = 0.0;
  frame.total_force_x = 0.0;
  frame.net_force.z = 0.0;
  frame.net_force.y = 0.0;
  frame.net_force.x = 0.0;
  frame.grip_factor = 1.0;
  if (car->wheels[1].in_contact != 0) {
    wheels_touching = wheels_touching + 1;
  }
  if (car->wheels[2].in_contact != 0) {
    wheels_touching = wheels_touching + 1;
  }
  if (car->wheels[3].in_contact != 0) {
    wheels_touching = wheels_touching + 1;
  }
  /* 18 contact records (first 4: wheel contacts): force along the contact normal, weight by wheel count / speed, sum up forces and torque */
  contact_next = (float *)((char *)&car->contacts[0] + 8);
  do {
    contact = contact_next;
    if (*(int *)&contact[-2] != -1) {
      frame.contact_force.z = contact[0xb];
      if (contact_index < 4) {
        frame.contact_force.z = frame.contact_force.z * k_f_n1;
        if (*contact < k_f_0_9) {
          frame.contact_force.z = frame.contact_force.z * k_f_4;
        }
      }
      else if (wheels_touching == 4) {
        frame.contact_force.z = frame.contact_force.z * k_f_n1;
      }
      else {
        frame.contact_force.z = frame.contact_force.z * k_f_n0_5;
      }
      tmp_f = contact[0xb] / car->speed;
      if (tmp_f < k_f_0) {
        tmp_f = -tmp_f;
        if (float_1_0 < tmp_f) {
          tmp_f = float_1_0;
        }
        frame.grip_factor = float_1_0 - tmp_f;
      }
      if ((*(int *)((char *)car + 0xcfc) != 0) && (frame.contact_force.z < k_f_500)) {
        frame.contact_force.z = k_f_500;
      }
      frame.contact_force.z = frame.contact_force.z * k_f_0_75;
      frame.contact_force.x = frame.contact_force.z * contact[-1];
      frame.contact_force.y = frame.contact_force.z * *contact;
      frame.contact_force.z = frame.contact_force.z * contact[1];
      frame.net_force.x = frame.contact_force.x + frame.net_force.x;
      frame.net_force.y = frame.contact_force.y + frame.net_force.y;
      frame.net_force.z = frame.contact_force.z + frame.net_force.z;
      frame.total_force_x = frame.contact_force.x + frame.total_force_x;
      frame.total_force_y = frame.contact_force.y + frame.total_force_y;
      frame.total_force_z = frame.contact_force.z + frame.total_force_z;
      if (*(int *)((char *)car + 0xcfc) == 0) {
        frame.extra_force.x = contact[-1] * k_f_90;
        frame.extra_force.y = *contact * k_f_90;
        frame.extra_force.z = contact[1] * k_f_90;
      }
      else {
        frame.extra_force.x = contact[-1] * k_f_500;
        frame.extra_force.y = *contact * k_f_500;
        frame.extra_force.z = contact[1] * k_f_500;
      }
      frame.net_force.x = frame.extra_force.x + frame.net_force.x;
      frame.net_force.y = frame.extra_force.y + frame.net_force.y;
      frame.net_force.z = frame.extra_force.z + frame.net_force.z;
      frame.total_force_x = frame.extra_force.x + frame.total_force_x;
      frame.total_force_y = frame.extra_force.y + frame.total_force_y;
      frame.total_force_z = frame.extra_force.z + frame.total_force_z;
      if (3 < contact_index) {
        RwV3dNormalize(&frame.normal_dir,(RwV3d *)(contact + 8));
        tmp_f = frame.normal_dir.x * contact[-1] + frame.normal_dir.y * *contact + frame.normal_dir.z * contact[1];
        vec_len = RwV3dLength(&frame.contact_force);
        tmp_f = vec_len * tmp_f;
        frame.normal_part_x = frame.normal_dir.x * tmp_f;
        frame.normal_part_y = frame.normal_dir.y * tmp_f;
        tmp_f = tmp_f * frame.normal_dir.z;
        frame.tangent_x = frame.contact_force.x - frame.normal_part_x;
        frame.tangent_y = frame.contact_force.y - frame.normal_part_y;
        frame.tangent_z = frame.contact_force.z - tmp_f;
        frame.torque_axis.x = frame.tangent_z * frame.normal_part_y - frame.tangent_y * tmp_f;
        frame.torque_axis.y = frame.tangent_x * tmp_f - frame.tangent_z * frame.normal_part_x;
        frame.torque_axis.z = frame.tangent_y * frame.normal_part_x - frame.tangent_x * frame.normal_part_y;
        if (k_f_0 < contact[10] * frame.contact_force.z +
                    frame.contact_force.x * ((RwV3d *)(contact + 8))->x + contact[9] * frame.contact_force.y) {
          frame.torque_axis.x = -frame.torque_axis.x;
          frame.torque_axis.y = -frame.torque_axis.y;
          frame.torque_axis.z = -frame.torque_axis.z;
        }
        frame.net_force.x = frame.normal_part_x + frame.net_force.x;
        frame.net_force.y = frame.normal_part_y + frame.net_force.y;
        frame.net_force.z = tmp_f + frame.net_force.z;
        frame.total_force_x = frame.contact_force.x + frame.total_force_x;
        frame.total_force_y = frame.contact_force.y + frame.total_force_y;
        frame.total_force_z = frame.contact_force.z + frame.total_force_z;
        frame.extra_force.z = contact[2];
        frame.extra_force.x = frame.tangent_x * frame.extra_force.z;
        frame.extra_force.y = frame.tangent_y * frame.extra_force.z;
        frame.extra_force.z = frame.tangent_z * frame.extra_force.z;
        vec_len = RwV3dLength(&frame.extra_force);
        tmp_f = vec_len * k_f_50_24;
        frame.tangent_force_sum = tmp_f + frame.tangent_force_sum;
        vec_len = RwV3dLength(&frame.torque_axis);
        if (k_f_1_0en4 < vec_len) {
          RwV3dNormalize(&frame.torque_axis,&frame.torque_axis);
          frame.normal_dir.x = frame.torque_axis.x * tmp_f;
          frame.normal_dir.y = frame.torque_axis.y * tmp_f;
          frame.normal_dir.z = frame.torque_axis.z * tmp_f;
          frame.torque_sum.x = frame.torque_sum.x - frame.normal_dir.x;
          frame.torque_sum.y = frame.torque_sum.y - frame.normal_dir.y;
          frame.torque_sum.z = frame.torque_sum.z - frame.normal_dir.z;
        }
      }
    }
    contact_index = contact_index + 1;
    contact_next = contact + 0x10;
  } while (contact_index < 0x12);
  /* average by the contact count, convert to velocity changes */
  tmp_f = float_1_0 / (float)*(int *)((char *)car + 0x9ec);
  frame.total_force_x = frame.total_force_x * tmp_f;
  frame.total_force_y = frame.total_force_y * tmp_f;
  frame.total_force_z = frame.total_force_z * tmp_f;
  frame.net_force.x = frame.net_force.x * tmp_f;
  frame.net_force.y = frame.net_force.y * tmp_f;
  frame.net_force.z = frame.net_force.z * tmp_f;
  frame.torque_sum.x = frame.torque_sum.x * tmp_f;
  frame.torque_sum.y = frame.torque_sum.y * tmp_f;
  frame.torque_sum.z = frame.torque_sum.z * tmp_f;
  frame.tangent_force_sum = frame.tangent_force_sum * tmp_f;
  vec_len = RwV3dLength(&frame.torque_sum);
  if (frame.tangent_force_sum - vec_len != k_f_0) {
    frame.extra_force.x = frame.total_force_x - frame.net_force.x;
    frame.extra_force.y = frame.total_force_y - frame.net_force.y;
    frame.extra_force.z = frame.total_force_z - frame.net_force.z;
  }
  tmp_f = car->inertia_inverse * k_f_0_0027778;
  *(undefined4 *)((char *)car + 0xb20) = 1;
  tmp_f = tmp_f * k_f_8;
  frame.torque_sum.x = frame.torque_sum.x * tmp_f;
  frame.torque_sum.y = frame.torque_sum.y * tmp_f;
  frame.torque_sum.z = tmp_f * frame.torque_sum.z;
  vec_len = RwV3dLength(&frame.torque_sum);
  if ((vec_len != k_f_0) && (vec_len < k_f_0_25)) {
    tmp_f = k_f_0_25 / vec_len;
    if (float_50_0 < k_f_0_25 / vec_len) {
      tmp_f = float_50_0;
    }
    frame.torque_sum.x = frame.torque_sum.x * tmp_f;
    frame.torque_sum.y = frame.torque_sum.y * tmp_f;
    frame.torque_sum.z = tmp_f * frame.torque_sum.z;
  }
  frame.torque_sum.x = frame.torque_sum.x * k_f_0_5;
  contact_next = car->velocity;
  frame.torque_sum.y = frame.torque_sum.y * k_f_0_5;
  frame.torque_sum.z = frame.torque_sum.z * k_f_0_5;
  car->velocity_input[0] = frame.torque_sum.x + car->velocity_input[0];
  car->velocity_input[1] = frame.torque_sum.y + car->velocity_input[1];
  car->velocity_input[2] = frame.torque_sum.z + car->velocity_input[2];
  tmp_f = *contact_next;
  *contact_next = frame.net_force.x + tmp_f;
  vel_y_new = frame.net_force.y + car->velocity[1];
  car->velocity[1] = vel_y_new;
  vel_z_new = frame.net_force.z + car->velocity[2];
  car->velocity[2] = vel_z_new;
  if (*(int *)&contact[-2] != -1) {
    damp = frame.grip_factor * k_f_3;
    if (k_f_0_9 < frame.grip_factor * k_f_3) {
      damp = k_f_0_9;
    }
    *contact_next = (frame.net_force.x + tmp_f) * damp;
    car->velocity[1] = vel_y_new * damp;
    car->velocity[2] = vel_z_new * damp;
  }
  if ((*(int *)&car->wheels_in_contact == 0x40800000) && (vec_len = RwV3dLength((RwV3d *)contact_next), k_f_1_0en4 < vec_len))
  {
    RwV3dNormalize(&frame.extra_force,(RwV3d *)contact_next);
    tmp_f = (frame.extra_force.z * car->up[2] + frame.extra_force.x * car->up[0] + frame.extra_force.y * car->up[1]) * k_f_0_1;
    frame.extra_force.z = tmp_f * vec_len;
    frame.extra_force.x = frame.extra_force.z * car->up[0];
    frame.extra_force.y = frame.extra_force.z * car->up[1];
    frame.extra_force.z = frame.extra_force.z * car->up[2];
    *contact_next = *contact_next - frame.extra_force.x;
    car->velocity[1] = car->velocity[1] - frame.extra_force.y;
    car->velocity[2] = car->velocity[2] - frame.extra_force.z;
    RwV3dLength(&frame.net_force);
  }
  return;
}

/* entry thunk at the original address of car_ground_contact_forces: register inputs (edi) -> natural C car_ground_contact_forces_n; ret 0 */
__declspec(naked) void car_ground_contact_forces(void)
{
    __asm {
        push edi
        call car_ground_contact_forces_n
        add esp, 4
        ret
    }
}
