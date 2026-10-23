// cluster-entries: car_points_vs_triangles
/* Installed cluster car_points_vs_triangles (tools/cluster_install.py from output/cluster_pilot_j1/car_points_vs_triangles.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040fdc0*/ extern int race_get_phase();
/*00477130*/ extern int __cdecl car_wheel_contact_find_index(undefined4 p0, undefined4 p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005ddc28*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de270*/ extern float k_f_n0_1;
/*005dfd64*/ extern float k_f_0_2777778;
/*005dfd68*/ extern float k_f_3_6;
/*005dfd6c*/ extern float k_f_0_42;
/*007e7f60*/ extern volatile undefined4 g_event_log_head;      /* volatile: the log arrays are indexed by it and the original reloads it after every store (index -1 aliases the head itself) */
/*007e7f64*/ extern volatile undefined4 g_event_log_type;
/*007e8364*/ extern volatile undefined4 g_event_log_time;
/*007e8764*/ extern volatile undefined4 g_event_log_param;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*008bd74c*/ extern undefined4 g_collision_triangle_count;
/* Tests the 18 collision points of a car (contact records of 0x40 bytes from car+0x4c0) against the collected triangles
   (0x90 bytes each from `triangles`+8: corners, normal at +0x1c, three edge planes, material colour at +0x2c).  A point that lies at most
   0.1 below a triangle plane and inside its edges gets the contact velocity / depth; the special material colours 0xff010101 and
   0xff0000ff write an event record (type 6) into the event log ring buffer (256 records, head `g_event_log_head`). */

/* "tests the car collision points (+0x27F...) against the collected triangles (inferred)" */

void car_points_vs_triangles(int *car,int triangles)

{
  float cos_in;
  undefined4 uVar2;
  float *tri_cur;
  int race_phase;
  int iVar4;
  float *tri_next;
  RwV3d *contact;
  RwReal spin_len;
  struct {
    RwV3d spin_vel;
    float *tri;
    float *point;
    float depth;
    float speed;
    float px;
    float py;
    float pz;
    int *point_state;
    int point_index;
    uint tri_index;
    float ny;
    float v0y;
    float v1x;
    float v2z;
    float nz;
    float v2y;
    float v1z;
    float v2x;
    float v0z;
    RwMatrix *body_matrix;
    float v1y;
    float v0x;
    float nx;
    RwMatrix spin_matrix;
    float zt;
  } frame;
  
  frame.body_matrix = (RwMatrix *)(car + car[0x26b] * 0x10 + 0x24a);
  frame.tri_index = 0;
  if (g_collision_triangle_count != 0) {
    tri_next = (float *)(triangles + 8);
    do {
      frame.v0x = tri_next[-2];
      frame.v0y = tri_next[-1];
      frame.v0z = *tri_next;
      frame.v1y = tri_next[2];
      frame.v2x = tri_next[4];
      frame.v2z = tri_next[6];
      frame.v1x = tri_next[1];
      frame.ny = tri_next[8];
      frame.v1z = tri_next[3];
      frame.v2y = tri_next[5];
      frame.nx = tri_next[7];
      frame.nz = tri_next[9];
      if (frame.ny < k_f_0_42) {
        frame.point = (float *)(car + 0x27f);
        frame.point_state = car + 0x66;
        frame.point_index = 0;
        contact = (RwV3d *)(car + 0x130);
        frame.tri = tri_next;
        do {
          if ((3 < frame.point_index) || (*frame.point_state == 0)) {
            frame.px = frame.point[-1];
            frame.py = *frame.point;
            frame.pz = frame.point[1];
            frame.depth = (frame.px - frame.v0x) * frame.nx +
                       (frame.py - frame.v0y) * frame.ny + frame.nz * (frame.pz - frame.v0z);
            if ((frame.depth < k_f_0) &&
               ((((k_f_0 <= (frame.pz - frame.v0z) * tri_next[0x1b] +
                            (frame.px - frame.v0x) * tri_next[0x19] +
                            (frame.py - frame.v0y) * tri_next[0x1a] &&
                  (k_f_0 <= (frame.px - frame.v1x) * tri_next[0x1c] +
                            (frame.py - frame.v1y) * tri_next[0x1d] +
                            (frame.pz - frame.v1z) * tri_next[0x1e])) &&
                 (k_f_0 <= (frame.px - frame.v2x) * tri_next[0x1f] +
                           (frame.py - frame.v2y) * tri_next[0x20] +
                           (frame.pz - frame.v2z) * tri_next[0x21])) && (k_f_n0_1 <= frame.depth)))) {
              contact[1].x = -frame.depth;
              *(volatile float *)&frame.zt = frame.point[-0x265] - *(float *)&car[0x59];
              frame.spin_vel.x = (frame.point[-0x267] - *(float *)&car[0x57]) * k_f_3_6;
              frame.spin_vel.y = (frame.point[-0x266] - *(float *)&car[0x58]) * k_f_3_6;
              frame.spin_vel.z = *(volatile float *)&frame.zt * k_f_3_6;
              RwV3dTransformVectors((RwV3d *)&contact[1].z,&frame.spin_vel,1,frame.body_matrix);
              if (((*(float *)&car[0x26f] == k_f_0) && (*(float *)&car[0x270] == k_f_0)) &&
                 (*(float *)&car[0x271] == k_f_0)) {
                frame.spin_vel.z = 0.0;
                frame.spin_vel.y = 0.0;
                frame.spin_vel.x = 0.0;
              }
              else {
                RwMatrixRotate(&frame.spin_matrix,(RwV3d *)(car + 0x26f),90.0,rwCOMBINEREPLACE);
                RwV3dTransformVectors(&frame.spin_vel,(RwV3d *)&contact[1].z,1,&frame.spin_matrix);
                RwV3dNormalize(&frame.spin_vel,&frame.spin_vel);
                spin_len = RwV3dLength((RwV3d *)(car + 0x26f));
                cos_in = spin_len * k_f_360 * contact[-1].z * k_f_0_2777778;
                frame.spin_vel.x = frame.spin_vel.x * cos_in;
                frame.spin_vel.y = frame.spin_vel.y * cos_in;
                frame.spin_vel.z = frame.spin_vel.z * cos_in;
              }
              contact->x = *(float *)&car[0x26c] + frame.spin_vel.x;
              contact->y = *(float *)&car[0x26d] + frame.spin_vel.y;
              contact->z = *(float *)&car[0x26e] + frame.spin_vel.z;
              frame.speed = RwV3dLength(contact);
              tri_next = frame.tri;
              if (k_f_1_0en4 < frame.speed) {
                RwV3dNormalize(contact,contact);
                tri_cur = frame.tri;
                cos_in = contact->y * frame.tri[8] + contact->x * frame.tri[7] + frame.tri[9] * contact->z;      /* the original adds y, x, z contact this order */
                tri_next = frame.tri;
                if (cos_in < k_f_0) {
                  if (cos_in < k_f_n1) {
                    cos_in = k_f_n1;
                  }
                  frame.speed = frame.speed * cos_in;
                  if (*(unsigned int *)&frame.tri[0xb] == 0xff010101u) {
                    if ((((car[0x27c] == 0) &&
                         (race_phase = race_get_phase(), uVar2 = g_last_frame_ticks,
                         tri_next = frame.tri, race_phase != 1)) &&
                        ((race_phase != 2 && ((race_phase != 3 && (race_phase != 4)))))) &&
                       (race_phase != 5)) {
                      (&g_event_log_type)[g_event_log_head] = 6;
                      (&g_event_log_time)[g_event_log_head] = uVar2;
                      (&g_event_log_param)[g_event_log_head] = *car + 0xff00;
                      g_event_log_args[g_event_log_head].arg0 = 0xff010101;
                      g_event_log_args[g_event_log_head].arg1 = ((unsigned int *)&frame.spin_matrix)[1];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].arg2 = ((unsigned int *)&frame.spin_matrix)[2];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w0c = frame.spin_matrix.flags;
                      g_event_log_args[g_event_log_head].w10 = ((unsigned int *)&frame.spin_matrix)[4];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w14 = ((unsigned int *)&frame.spin_matrix)[5];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w18 = ((unsigned int *)&frame.spin_matrix)[6];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w1c = frame.spin_matrix.pad1;
                      race_phase = 0;
                      do {
                        iVar4 = race_phase + 1;
                        (&g_event_log_data[g_event_log_head].w00)[race_phase] =
                             (&frame.spin_matrix.right.x)[race_phase];
                        race_phase = iVar4;
                      } while (iVar4 < 0x10);
                      g_event_log_head = g_event_log_head + 1;
                      if (0xffu < (unsigned int)g_event_log_head) {
                        g_event_log_head = 0;
                      }
                    }
                  }
                  else {
                    if (((((*(unsigned int *)&frame.tri[0xb] == 0xff0000ffu) &&
                          ((car[0x27c] == 0 || (car[0x27c] == 1)))) &&
                         (race_phase = race_get_phase(), uVar2 = g_last_frame_ticks, race_phase != 1
                         )) && (((race_phase != 2 && (race_phase != 3)) && (race_phase != 4)))) &&
                       (race_phase != 5)) {
                      (&g_event_log_type)[g_event_log_head] = 6;
                      (&g_event_log_time)[g_event_log_head] = uVar2;
                      (&g_event_log_param)[g_event_log_head] = *car + 0xff00;
                      g_event_log_args[g_event_log_head].arg0 = 0xff0000ff;
                      g_event_log_args[g_event_log_head].arg1 = ((unsigned int *)&frame.spin_matrix)[1];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].arg2 = ((unsigned int *)&frame.spin_matrix)[2];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w0c = frame.spin_matrix.flags;
                      g_event_log_args[g_event_log_head].w10 = ((unsigned int *)&frame.spin_matrix)[4];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w14 = ((unsigned int *)&frame.spin_matrix)[5];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w18 = ((unsigned int *)&frame.spin_matrix)[6];      /* raw bits of the matrix word (not a float conversion) */
                      g_event_log_args[g_event_log_head].w1c = frame.spin_matrix.pad1;
                      race_phase = 0;
                      do {
                        iVar4 = race_phase + 1;
                        (&g_event_log_data[g_event_log_head].w00)[race_phase] =
                             (&frame.spin_matrix.right.x)[race_phase];
                        race_phase = iVar4;
                      } while (iVar4 < 0x10);
                      g_event_log_head = g_event_log_head + 1;
                      if (0xffu < (unsigned int)g_event_log_head) {
                        g_event_log_head = 0;
                      }
                    }
                    race_phase = car_wheel_contact_find_index((undefined4)(tri_cur + -2),(undefined4)car);
                    tri_next = frame.tri;
                    if (race_phase == 0) {
                      contact[2].z = frame.speed;
                      contact[-2].z = tri_cur[7];
                      contact[-1].x = tri_cur[8];
                      contact[-1].y = tri_cur[9];
                      contact[-2].x = frame.depth;
                      contact[-2].y = tri_cur[10];
                    }
                  }
                }
              }
            }
          }
          frame.point_index = frame.point_index + 1;
          frame.point_state = frame.point_state + 0x31;
          frame.point = frame.point + 3;
          contact = (RwV3d *)&contact[5].y;
        } while (frame.point_index < 0x12);
      }
      frame.tri_index = frame.tri_index + 1;
      tri_next = tri_next + 0x24;
    } while (frame.tri_index < g_collision_triangle_count);
  }
  return;
}

