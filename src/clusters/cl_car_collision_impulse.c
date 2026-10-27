// cluster-entries: car_collision_impulse
/* Installed cluster car_collision_impulse (tools/cluster_install.py from output/cluster_pilot_j1/car_collision_impulse.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00422090*/ extern int vehicle_get_matrix();
/*00473ac0*/ extern int __cdecl course_bbox_transform_and_test(struct RwBBox *p0, RwMatrix *p1, RwReal *p2, RwMatrix *p3, struct RwV3d *p4);
/*0047b520*/ extern int car_current_matrix();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd62c*/ extern float k_f_100;
/*005ddc28*/ extern float k_f_0;
/*005df458*/ extern float k_f_200;
/*005dfd70*/ extern float k_f_2_0en7;
/*008b06e0*/ extern struct Car player_car[16];
/* car_collision_impulse */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* resolves a hit at a contact point: updates the velocity at +0x9B0 and returns the impulse */

#pragma auto_inline(off)
static float float_barrier(float v) { return v; }
#pragma auto_inline(on)

undefined4
car_collision_impulse(int car_index,RwBBox *obstacle_box,RwMatrix *box_matrix,float obstacle_mass,float *out_impulse)

{
  float *car_pos;
  float inv_total_mass;
  float separation_push;
  int hit;
  double len_other;
  double dot, t, X2, Y2, Z2, px, py, pz;
  float len_v;
  struct {
    RwMatrix *car_matrix_ptr [2];
    float box_center_x;
    float box_center_y;
    float box_center_z;
    RwV3d contact_point;
    RwV3d car_momentum;
    RwV3d other_momentum;
    RwV3d impulse_car;
    RwV3d impulse_other;
    RwV3d dir_car_to_contact;
    RwV3d dir_center_to_contact;
    RwReal car_world_matrix [6];
  } frame;
  
  /* centre of the obstacle box */
  frame.box_center_x = ((obstacle_box->inf).x + (obstacle_box->sup).x) * k_f_0_5;
  car_pos = (float *)(player_car[car_index].matrix_current * 0x40 + car_index * 0xd04 + 0x8b1038);
  frame.contact_point.z = 0.0;
  frame.box_center_y = ((obstacle_box->inf).y + (obstacle_box->sup).y) * k_f_0_5;
  frame.contact_point.y = 0.0;
  frame.contact_point.x = 0.0;
  frame.box_center_z = ((obstacle_box->inf).z + (obstacle_box->sup).z) * k_f_0_5;
  vehicle_get_matrix(car_index,frame.car_world_matrix);
  car_current_matrix(frame.car_matrix_ptr,car_index);
  /* contact point of the car box with the obstacle box (also tests that they overlap) */
  hit = course_bbox_transform_and_test(obstacle_box,box_matrix,frame.car_world_matrix,frame.car_matrix_ptr[0],&frame.contact_point);
  if (hit != 0) {
    /* only when the car moves towards the contact point: momentum exchange along the contact, summed into impulse_car / impulse_other */
    frame.dir_car_to_contact.x = frame.contact_point.x - *car_pos;
    frame.dir_car_to_contact.y = frame.contact_point.y - car_pos[1];
    frame.dir_car_to_contact.z = frame.contact_point.z - car_pos[2];
    RwV3dNormalize(&frame.dir_car_to_contact,&frame.dir_car_to_contact);
    if (k_f_0 <= frame.dir_car_to_contact.z * player_car[car_index].velocity[2] +
                 frame.dir_car_to_contact.x * player_car[car_index].velocity[0] +
                 frame.dir_car_to_contact.y * player_car[car_index].velocity[1]) {
      frame.dir_center_to_contact.x = frame.contact_point.x - frame.box_center_x;
      frame.dir_center_to_contact.y = frame.contact_point.y - frame.box_center_y;
      frame.dir_center_to_contact.z = frame.contact_point.z - frame.box_center_z;
      RwV3dNormalize(&frame.dir_center_to_contact,&frame.dir_center_to_contact);
      frame.car_momentum.x = player_car[car_index].mass * player_car[car_index].velocity[0];
      frame.impulse_car.z = 0.0;
      frame.impulse_car.y = 0.0;
      frame.impulse_car.x = 0.0;
      frame.impulse_other.z = 0.0;
      frame.impulse_other.y = 0.0;
      frame.car_momentum.y = player_car[car_index].velocity[1] * player_car[car_index].mass;
      frame.impulse_other.x = 0.0;
      frame.other_momentum.z = 0.0;
      frame.other_momentum.y = 0.0;
      frame.other_momentum.x = 0.0;
      frame.car_momentum.z = player_car[car_index].mass * player_car[car_index].velocity[2];
      len_v = RwV3dLength(&frame.car_momentum);
      len_other = RwV3dLength(&frame.other_momentum);       /* stays in the x87 stack (not rounded) in the original */
      if (len_v != k_f_0) {
        dot = (double)frame.other_momentum.y * frame.car_momentum.y + (double)frame.other_momentum.x * frame.car_momentum.x
            + (double)frame.other_momentum.z * frame.car_momentum.z;
        t = dot * ((double)float_1_0 / len_v);
        if (len_other != k_f_0) {
          t = t / len_other;
        }
        t = -(((double)float_1_0 / len_v) * t * len_other);
        if (t > k_f_0) t = k_f_0;
        else if (t < k_f_n1) t = k_f_n1;
        px = float_barrier(frame.car_momentum.x * t);
        py = float_barrier(frame.car_momentum.y * t);
        pz = t * frame.car_momentum.z;                 /* the z product is not rounded */
        frame.impulse_other.x = (float)(px + frame.car_momentum.x);
        frame.impulse_other.y = float_barrier(py + frame.car_momentum.y);
        frame.impulse_other.z = float_barrier(pz + frame.car_momentum.z);
        frame.impulse_car.x = -(float)(px + frame.car_momentum.x);
        frame.impulse_car.y = -frame.impulse_other.y;
        frame.impulse_car.z = -frame.impulse_other.z;
      }
      if (len_other != k_f_0) {
        dot = (double)frame.other_momentum.y * frame.car_momentum.y + (double)frame.other_momentum.x * frame.car_momentum.x
            + (double)frame.other_momentum.z * frame.car_momentum.z;
        t = dot * ((double)float_1_0 / len_other);
        if (len_v != k_f_0) {
          t = t / len_v;
        }
        t = -(((double)float_1_0 / len_other) * t * len_v);
        if (t > k_f_0) t = k_f_0;
        else if (t < k_f_n1) t = k_f_n1;
        px = float_barrier(frame.other_momentum.x * t);
        py = float_barrier(frame.other_momentum.y * t);
        pz = t * frame.other_momentum.z;
        X2 = px + frame.other_momentum.x;                /* unrounded */
        Y2 = float_barrier(py + frame.other_momentum.y);
        Z2 = float_barrier(pz + frame.other_momentum.z);
        frame.impulse_car.x = frame.impulse_car.x + X2;
        frame.impulse_car.y = Y2 + frame.impulse_car.y;
        frame.impulse_car.z = Z2 + frame.impulse_car.z;
        frame.impulse_other.x = frame.impulse_other.x - X2;
        frame.impulse_other.y = frame.impulse_other.y - Y2;
        frame.impulse_other.z = frame.impulse_other.z - Z2;
      }
      inv_total_mass = float_1_0 / (obstacle_mass + player_car[car_index].mass);
      player_car[car_index].velocity[0] = frame.impulse_car.x * inv_total_mass + player_car[car_index].velocity[0];
      player_car[car_index].velocity[1] = frame.impulse_car.y * inv_total_mass + player_car[car_index].velocity[1];
      player_car[car_index].velocity[2] = frame.impulse_car.z * inv_total_mass + player_car[car_index].velocity[2];
      *out_impulse = frame.impulse_other.x * inv_total_mass;
      out_impulse[1] = frame.impulse_other.y * inv_total_mass;
      out_impulse[2] = inv_total_mass * frame.impulse_other.z;
      len_v = RwV3dLength(&frame.impulse_car);
      len_other = RwV3dLength(&frame.impulse_other);
      inv_total_mass = (len_other + len_v) * k_f_2_0en7;
      if (float_1_0 < inv_total_mass) {
        inv_total_mass = float_1_0;
      }
      separation_push = inv_total_mass * k_f_100;
      if (inv_total_mass * k_f_100 < k_f_200) {
        separation_push = k_f_200;
      }
      player_car[car_index].velocity[0] = player_car[car_index].velocity[0] - frame.dir_car_to_contact.x * separation_push;
      player_car[car_index].velocity[1] = player_car[car_index].velocity[1] - frame.dir_car_to_contact.y * separation_push;
      player_car[car_index].velocity[2] = player_car[car_index].velocity[2] - frame.dir_car_to_contact.z * separation_push;
      *out_impulse = *out_impulse - frame.dir_center_to_contact.x * separation_push;
      out_impulse[1] = out_impulse[1] - frame.dir_center_to_contact.y * separation_push;
      out_impulse[2] = out_impulse[2] - frame.dir_center_to_contact.z * separation_push;
      /* (inferred) hit timer / cooldown set to 480 */
      *(int *)((char *)&player_car[car_index] + 0xaec) = 0x1e0;
      return 1;
    }
  }
  return 0;
}

