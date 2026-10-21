// cluster-entries: car_car_collision
/* Installed cluster car_car_collision (tools/cluster_install.py from output/cluster_pilot_uc3/car_car_collision.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase(void);
/*004d5290*/ extern float RwV3dNormalize(RwV3d *out, RwV3d *in);
/*004d53a0*/ extern float RwV3dLength(RwV3d *v);
/*004d4a30*/ extern RwMatrix *RwMatrixInvert(RwMatrix *inverse, RwMatrix *matrix);
/*004d56d0*/ extern RwV3d *RwV3dTransformVectors(RwV3d *out, RwV3d *in, int count, RwMatrix *matrix);
extern int g_game_801b08;
extern unsigned int g_event_log_head;
extern int g_event_log_type[256];
extern int g_event_log_time[256];
extern int g_event_log_param[256];
extern int g_last_frame_ticks;
extern struct EventLogArgs g_event_log_args[256];
/* the event log data record (0x40 bytes): all fields are floats (positions, velocities, impact strength) */
typedef struct EventLogDataF {
  float w00, w04, w08, w0c, w10, w14, w18, w1c, w20, w24, w28, w2c, w30, w34, w38, w3c;
} EventLogDataF;
extern EventLogDataF g_event_log_data[256];

/* float word `i` of a car (the physics fields the file does not name) */
#define CF(car, i) (((float *)(car))[i])
#define CU(car, i) (((unsigned int *)(car))[i])
/* car fields by word index */
#define HULL_UP 0x272          /* up vector x y z */
#define HULL_P0 0x28a          /* four hull corner points, 3 floats each: 0x28a, 0x28d, 0x290, 0x293 */
#define VELOCITY 0x26c         /* velocity x y z */
#define VELOCITY_INPUT 0x51    /* velocity_input x y z, y at 0x52 */
#define ANGULAR_VELOCITY 0x26f /* angular velocity x y z */

/* Collision response between two cars (a = ecx, b = eax): each car's four hull corner points are tested against the other car's tetrahedron (four planes);
   the mean of the points inside is the contact point (nothing inside: no collision, returns 0).  The cars then exchange velocity along the contact normals
   (elasticity from the normal speeds), are pushed apart (impulse scaled by the speed sum, at least 200), get a yaw spin from the contact side, and the
   event is logged (type 4, 16 floats) unless the race is in phase 1..5.  Returns param_4 == 0. */
int car_car_collision_n(Car *a, Car *b, int unused, int param_4)
{
  float *pos_a, *pos_b;
  float contact_x, contact_y, contact_z;
  int inside_count;
  int inside_per_car[2];
  unsigned int u;
  Car *hull, *other;
  float ux, uy, uz;
  float p0x, p0y, p0z, p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z;
  RwV3d n0, n1;
  float n2x, n2y, n2z;
  float n3x, n3y, n3z;
  float ex, ez;
  float inv;
  RwV3d dir_a, dir_b, vel_a, vel_b, push_a, push_b, at_a, at_b;
  RwV3d *va, *vb;
  float dot, len_a, len_b, len, t, r, py, pz, spin;
  float strength, sum;
  RwMatrix inverse;
  float log_data[16];
  int different;
  Car *first, *second;
  unsigned int stale_88, stale_84;       /* never written: the original copies two stale stack words into the event record */
  int i;

  pos_b = &CF(b, 0x256 + CU(b, 0x26a) * 0x10);
  pos_a = &CF(a, 0x256 + CU(a, 0x26a) * 0x10);
  inside_count = 0;
  inside_per_car[0] = 0;
  inside_per_car[1] = 0;
  contact_z = 0.0f;
  contact_y = 0.0f;
  contact_x = 0.0f;
  for (u = 0; u < 2; u++) {
    hull = (u == 0) ? b : a;
    other = (u == 0) ? a : b;
    ux = CF(hull, 0x272);
    uy = CF(hull, 0x273);
    uz = CF(hull, 0x274);
    p0x = CF(hull, 0x28a); p0y = CF(hull, 0x28b); p0z = CF(hull, 0x28c);
    p1x = CF(hull, 0x28d); p1y = CF(hull, 0x28e); p1z = CF(hull, 0x28f);
    p3x = CF(hull, 0x293); p3y = CF(hull, 0x294); p3z = CF(hull, 0x295);
    p2x = CF(hull, 0x290); p2y = CF(hull, 0x291); p2z = CF(hull, 0x292);
    n0.x = (p1z - p0z) * uy - (p1y - p0y) * uz;
    n0.y = (p1x - p0x) * uz - (p1z - p0z) * ux;
    n0.z = (p1y - p0y) * ux - (p1x - p0x) * uy;
    n1.x = (p3z - p1z) * uy - (p3y - p1y) * uz;
    n1.y = (p3x - p1x) * uz - (p3z - p1z) * ux;
    n1.z = (p3y - p1y) * ux - (p3x - p1x) * uy;
    n2x = (p2z - p3z) * uy - (p2y - p3y) * uz;
    n2y = (p2x - p3x) * uz - (p2z - p3z) * ux;
    n2z = (p2y - p3y) * ux - (p2x - p3x) * uy;
    ex = p0y - p2y;
    ez = p0z - p2z;
    n3x = ez * uy - ex * uz;
    n3y = (p0x - p2x) * uz - ez * ux;
    n3z = ex * ux - (p0x - p2x) * uy;
    /* hull planes: 0 (p0,p1), 1 (p1,p3), 2 (p3,p2), 3 (p2,p0) */
    for (i = 0; i < 4; i++) {
      float qx0, qy0, qz0;
      static const int point_word[4] = { 0x28a, 0x28d, 0x290, 0x293 };
      qx0 = CF(other, point_word[i]);
      qy0 = CF(other, point_word[i] + 1);
      qz0 = CF(other, point_word[i] + 2);
      if ((qx0 - p0x) * n0.x + (qy0 - p0y) * n0.y + n0.z * (qz0 - p0z) < 0.0f &&
          (qx0 - p1x) * n1.x + (qy0 - p1y) * n1.y + n1.z * (qz0 - p1z) < 0.0f &&
          (qx0 - p3x) * n2x + (qy0 - p3y) * n2y + n2z * (qz0 - p3z) < 0.0f &&
          (qx0 - p2x) * n3x + (qy0 - p2y) * n3y + n3z * (qz0 - p2z) < 0.0f) {
        contact_x = contact_x + qx0;
        inside_count = inside_count + 1;
        contact_y = qy0 + contact_y;
        if (i < 2) {
          inside_per_car[u] = inside_per_car[u] + 1;
        }
        contact_z = qz0 + contact_z;
      }
    }
  }
  if (inside_count == 0) {
    return 0;
  }
  inv = 1.0f / (float)(unsigned int)inside_count;
  contact_x = inv * contact_x;
  contact_y = inv * contact_y;
  contact_z = inv * contact_z;
  dir_a.x = contact_x - pos_a[0];
  dir_a.y = contact_y - pos_a[1];
  dir_a.z = contact_z - pos_a[2];
  at_a = dir_a;
  RwV3dNormalize(&dir_a, &dir_a);
  dir_b.x = contact_x - pos_b[0];
  dir_b.y = contact_y - pos_b[1];
  dir_b.z = contact_z - pos_b[2];
  at_b = dir_b;
  RwV3dNormalize(&dir_b, &dir_b);
  va = (RwV3d *)&CF(a, VELOCITY);
  vb = (RwV3d *)&CF(b, VELOCITY);
  vel_a.x = CF(a, VELOCITY_INPUT) * 600.0f + CF(a, VELOCITY);
  push_b.z = 0.0f;
  push_b.y = 0.0f;
  push_b.x = 0.0f;
  push_a.z = 0.0f;
  push_a.y = 0.0f;
  push_a.x = 0.0f;
  vel_a.y = CF(a, VELOCITY_INPUT + 1) * 600.0f + CF(a, VELOCITY + 1);
  vel_a.z = CF(a, VELOCITY_INPUT + 2) * 600.0f + CF(a, VELOCITY + 2);
  vel_b.x = CF(b, VELOCITY_INPUT) * 600.0f + CF(b, VELOCITY);
  vel_b.y = CF(b, VELOCITY_INPUT + 1) * 600.0f + CF(b, VELOCITY + 1);
  vel_b.z = CF(b, VELOCITY_INPUT + 2) * 600.0f + CF(b, VELOCITY + 2);
  len_a = RwV3dLength(&vel_a);
  len_b = RwV3dLength(&vel_b);
  dot = vel_b.z * vel_a.z + vel_b.x * vel_a.x + vel_b.y * vel_a.y;
  if (len_a != 0.0f) {
    t = dot * (1.0f / len_a);
    if (len_b != 0.0f) {
      t = t / len_b;
    }
    t = -((1.0f / len_a) * len_b * t);
    r = 0.0f;
    if (!(t > 0.0f)) {
      r = t;
      if (t < -1.0f) {
        r = -1.0f;
      }
    }
    r = r + 1.0f;
    push_b.x = vel_a.x * r;
    py = r * vel_a.y;
    pz = r * vel_a.z;
    push_a.x = -push_b.x;
    push_a.y = -py;
    push_a.z = -pz;
    push_b.y = py;
    push_b.z = pz;
  }
  if (len_b != 0.0f) {
    t = dot * (1.0f / len_b);
    if (len_a != 0.0f) {
      t = t / len_a;
    }
    t = -((1.0f / len_b) * len_a * t);
    r = 0.0f;
    if (!(t > 0.0f)) {
      r = t;
      if (t < -1.0f) {
        r = -1.0f;
      }
    }
    r = r + 1.0f;
    py = r * vel_b.y;
    pz = r * vel_b.z;
    push_a.x = push_a.x + vel_b.x * r;
    push_a.y = push_a.y + py;
    push_a.z = push_a.z + pz;
    push_b.x = push_b.x - vel_b.x * r;
    push_b.y = push_b.y - py;
    push_b.z = push_b.z - pz;
  }
  va->x = push_a.x * 0.5f + va->x;
  CF(a, VELOCITY + 1) = push_a.y * 0.5f + CF(a, VELOCITY + 1);
  CF(a, VELOCITY + 2) = push_a.z * 0.5f + CF(a, VELOCITY + 2);
  vb->x = push_b.x * 0.5f + vb->x;
  CF(b, VELOCITY + 1) = push_b.y * 0.5f + CF(b, VELOCITY + 1);
  CF(b, VELOCITY + 2) = push_b.z * 0.5f + CF(b, VELOCITY + 2);
  len = RwV3dLength(&push_a);
  len_b = RwV3dLength(&push_b);
  sum = len_b + len;
  if (g_game_801b08 < 3) {
    len = 100.0f;
  }
  if (g_game_801b08 == 3 || g_game_801b08 == 4) {
    len = 200.0f;
  }
  strength = sum * 0.0000002f;
  if (1.0f < sum * 0.0000002f) {
    strength = 1.0f;
  }
  strength = strength * len;
  if (strength < 200.0f) {
    strength = 200.0f;
  }
  RwV3dNormalize(&n0, va);
  RwV3dNormalize(&n1, vb);
  n0.x = n0.x - dir_a.x;
  n0.y = n0.y - dir_a.y;
  n0.z = n0.z - dir_a.z;
  n1.x = n1.x - dir_b.x;
  n1.y = n1.y - dir_b.y;
  n1.z = n1.z - dir_b.z;
  RwV3dNormalize(&n0, &n0);
  RwV3dNormalize(&n1, &n1);
  len = strength * -0.5f;
  va->x = va->x - n0.x * len;
  CF(a, VELOCITY + 1) = CF(a, VELOCITY + 1) - n0.y * len;
  CF(a, VELOCITY + 2) = CF(a, VELOCITY + 2) - n0.z * len;
  vb->x = vb->x - n1.x * len;
  CF(b, VELOCITY + 1) = CF(b, VELOCITY + 1) - n1.y * len;
  CF(b, VELOCITY + 2) = CF(b, VELOCITY + 2) - len * n1.z;
  va->x = va->x - dir_a.x * strength;
  CF(a, VELOCITY + 1) = CF(a, VELOCITY + 1) - dir_a.y * strength;
  CF(a, VELOCITY + 2) = CF(a, VELOCITY + 2) - dir_a.z * strength;
  vb->x = vb->x - dir_b.x * strength;
  CF(b, VELOCITY + 1) = CF(b, VELOCITY + 1) - dir_b.y * strength;
  CF(b, VELOCITY + 2) = CF(b, VELOCITY + 2) - dir_b.z * strength;
  if (CU(a, 699) == 0) {
    CU(a, 699) = 0x1e0;
  }
  if (CU(b, 699) == 0) {
    CU(b, 699) = 0x1e0;
  }
  RwMatrixInvert(&inverse, (RwMatrix *)&CF(a, 0x24a + CU(a, 0x26b) * 0x10));
  RwV3dTransformVectors(&dir_a, &at_a, 1, &inverse);
  RwMatrixInvert(&inverse, (RwMatrix *)&CF(b, 0x24a + CU(b, 0x26b) * 0x10));
  RwV3dTransformVectors(&dir_b, &at_b, 1, &inverse);
  spin = strength;
  if (0.0f <= dir_a.x) {
    if (0.0f < dir_a.z) {
      spin = -strength;
    }
  } else if (dir_a.z < 0.0f) {
    spin = -strength;
  }
  if (CU(a, 4) == 0) {
    CF(a, 0x52) = spin * 0.0005f + CF(a, 0x52);
  } else {
    spin = spin * 0.01f;
    CU(a, 0x26f) = 0;
    CU(a, 0x271) = 0;
    CF(a, 0x270) = spin;
  }
  if (0.0f <= dir_b.x) {
    if (0.0f < dir_b.z) {
      strength = -strength;
    }
  } else if (dir_b.z < 0.0f) {
    strength = -strength;
  }
  if (CU(b, 4) == 0) {
    CF(b, 0x52) = strength * 0.0005f + CF(b, 0x52);
  } else {
    strength = strength * 0.01f;
    CU(b, 0x26f) = 0;
    CU(b, 0x271) = 0;
    CF(b, 0x270) = strength;
  }
  log_data[0] = contact_x;
  log_data[1] = contact_y;
  log_data[2] = contact_z;
  different = inside_per_car[0] != inside_per_car[1];
  if (inside_per_car[0] < inside_per_car[1]) {
    first = b;
    second = a;
    log_data[3] = dir_b.x; log_data[4] = dir_b.y; log_data[5] = dir_b.z;
    log_data[9] = push_b.x; log_data[10] = push_b.y; log_data[11] = push_b.z;
    log_data[6] = dir_a.x; log_data[7] = dir_a.y; log_data[8] = dir_a.z;
    log_data[12] = push_a.x; log_data[13] = push_a.y; log_data[14] = push_a.z;
  } else {
    first = a;
    second = b;
    log_data[3] = dir_a.x; log_data[4] = dir_a.y; log_data[5] = dir_a.z;
    log_data[9] = push_a.x; log_data[10] = push_a.y; log_data[11] = push_a.z;
    log_data[6] = dir_b.x; log_data[7] = dir_b.y; log_data[8] = dir_b.z;
    log_data[12] = push_b.x; log_data[13] = push_b.y; log_data[14] = push_b.z;
  }
  {
    unsigned int id_first = CU(first, 0);
    unsigned int id_second = CU(second, 0);
    int phase = race_get_phase();
    int ticks = g_last_frame_ticks;

    if (phase != 1 && phase != 2 && phase != 3 && phase != 4 && phase != 5) {
      g_event_log_type[g_event_log_head] = 4;
      g_event_log_time[g_event_log_head] = ticks;
      g_event_log_param[g_event_log_head] = CU(first, 0) + 0xff00;
      g_event_log_args[g_event_log_head].arg0 = id_first;
      g_event_log_args[g_event_log_head].arg1 = id_second;
      g_event_log_args[g_event_log_head].arg2 = (unsigned int)different;
      g_event_log_args[g_event_log_head].w0c = *(unsigned int *)&at_b.x;
      g_event_log_args[g_event_log_head].w10 = *(unsigned int *)&at_b.y;
      g_event_log_args[g_event_log_head].w14 = *(unsigned int *)&at_b.z;
      g_event_log_args[g_event_log_head].w18 = stale_88;
      g_event_log_args[g_event_log_head].w1c = stale_84;
      for (i = 0; i < 0x10; i++) {
        (&g_event_log_data[g_event_log_head].w00)[i] = log_data[i];
      }
    } else {
      return param_4 == 0;
    }
  }
  g_event_log_head = g_event_log_head + 1;
  if (0xff < g_event_log_head) {
    g_event_log_head = 0;
  }
  return param_4 == 0;
}

/* entry thunk at the original address of car_car_collision: register inputs (ecx,eax,s0,s1) -> natural C car_car_collision_n; ret 0 */
__declspec(naked) void car_car_collision(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        push ecx
        call car_car_collision_n
        add esp, 16
        ret
    }
}
