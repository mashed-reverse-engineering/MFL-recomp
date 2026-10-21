// cluster-entries: car_wheel_contact_update
/* Installed cluster car_wheel_contact_update (tools/cluster_install.py from output/cluster_pilot_uc3/car_wheel_contact_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
typedef struct RpWorld RpWorld;
/*0047acc0*/ extern void car_wheels_vs_triangles(Car *car, void *triangles);
/*004b3284*/ extern double FUN_004b3284(double x);
/*004d5290*/ extern float RwV3dNormalize(RwV3d *out, RwV3d *in);
/*004d5670*/ extern RwV3d *RwV3dTransformPoints(RwV3d *out, RwV3d *in, int count, RwMatrix *matrix);
/*004d43f0*/ extern RwMatrix *RwMatrixRotate(RwMatrix *matrix, RwV3d *axis, float angle, enum RwOpCombineType combine);
/*004d49c0*/ extern RwMatrix *RwMatrixTransform(RwMatrix *matrix, RwMatrix *transform, enum RwOpCombineType combine);
/*00549290*/ extern RpWorld *RpCollisionWorldForAllIntersections(void *collision_world, void *intersection, void *callback, void *data);
extern void LAB_00477170(void);
extern int g_car_8bd790;
extern int g_car_8bd720[4];
extern RwV3d g_car_8b06a0[4];       /* the wheel contact probe points in car space (x, 0, z of the wheel) scaled by 0.2778 */
extern RwV3d g_car_8bd740;          /* the car centre transformed to the world */
extern RwV3d g_car_8bd760[4];       /* the probe points transformed to the world */
extern int g_collision_triangle_count;
extern void *g_car_8bd794;
extern char g_collision_triangles[];
extern float s_333_Lap_frames_005df368;
extern float g_car_62ac74, g_car_62ac78, g_car_62ac7c;

#define CAR_FLOAT(car, off) (*(float *)((char *)(car) + (off)))
#define WHEEL_PREVIOUS(car, i) ((float *)((char *)(car) + 0x9f8 + 12 * (i)))      /* 3 floats of wheel i: position from the previous step */
#define SURFACE_RESET_COLOUR 0xff020202       /* the surface colour of a track "reset" triangle */

/* An intersection query for RpCollisionWorldForAllIntersections: a sphere (centre, radius), type 3. */
typedef struct SphereQuery { float x, y, z, radius; int pad[2]; int type; } SphereQuery;

/* Pushes the car out of the track after the collision: gathers the track triangles around the car (sphere query), lets car_wheels_vs_triangles update the
   wheels' contact_distance / in_contact / contact_normal, then moves the car matrix (the previous one of the double buffer) by the mean penetration of the
   wheels in contact (when all four touch, the worst one is dropped), aligns the car to the surface normal (angle capped at 2.5 degrees when `limit_angle`)
   and applies the bounce / reset surface effects to the velocity; finally counts the wheels in contact into car->wheels_in_contact. */
static void triangle_normal_r(float * p0, float * p1, float * p2, float * p3);
extern void triangle_normal(void);
void car_wheel_contact_update_n(void *world, Car *car, int limit_angle)
{
  RwMatrix *matrix;
  RwV3d zero_point;
  SphereQuery query;
  RwMatrix rotation;
  RwV3d cross;
  RwV3d normal;                 /* mean / triangle normal of the wheels in contact */
  RwV3d saved_pos, tmp_vec;
  float delta_x[4], delta_y[4], delta_z[4];       /* normal * contact distance per wheel */
  float points[12];                               /* the probe points moved out by that */
  int wheels_before;
  int sunk, lifted, can_lift;
  int i, k;
  unsigned char count;
  double sum_x;                    /* kept unrounded on the x87 stack in the original */
  volatile float sum_y, sum_z;     /* stored 32 bit float locals in the original (read back from the stack by the position update) */
  float d, d0, d1, d2, d3, best;
  int worst;
  float angle, inv;
  double cosine, cosine_clamped;       /* stays an unrounded x87 value in the original */
  float speed_dot;
  Wheel *w;
  volatile float push[3];      /* stored float temporaries of the original */
  volatile float side_z;       /* a stored float in the original: the z component is rounded before it is scaled */
  volatile float push_scale;

  matrix = (RwMatrix *)(car->matrices + car->matrix_previous * 0x10);
  g_car_8bd790 = 0;
  zero_point.x = 0.0f;
  zero_point.y = 0.0f;
  zero_point.z = 0.0f;
  wheels_before = (int)car->wheels_in_contact;
  for (i = 0; i < 4; i++) {
    g_car_8bd720[i] = 0;
    w = &car->wheels[i];
    g_car_8b06a0[i].x = w->pos_x * 0.2778f;
    w->contact_triangle = -1;
    w->contact_distance = 10.0f;
    g_car_8b06a0[i].y = 0.0f;
    g_car_8b06a0[i].z = w->pos_z * 0.2778f;
  }
  RwV3dTransformPoints(&g_car_8bd740, &zero_point, 1, matrix);
  RwV3dTransformPoints(g_car_8bd760, g_car_8b06a0, 4, matrix);
  triangle_normal_r(&g_car_8bd760[0].x, &g_car_8bd760[2].x, &g_car_8bd760[3].x, car->up);
  query.type = 3;
  query.x = matrix->pos.x;
  query.y = matrix->pos.y;
  query.radius = car->bounds_radius;
  query.z = matrix->pos.z;
  g_collision_triangle_count = 0;
  g_car_8bd794 = world;
  RpCollisionWorldForAllIntersections(*(void **)((char *)world + 8), &query, LAB_00477170, g_collision_triangles);
  car_wheels_vs_triangles(car, g_collision_triangles);

  /* wheels in contact that are lifted (>= 0.02) or sunk (<= -0.005) */
  lifted = 0;
  sunk = 0;
  for (i = 0; i < 4; i++) {
    w = &car->wheels[i];
    if (w->in_contact == 1) {
      d = w->contact_distance;
      if (0.02f <= d) {
        lifted++;
      }
      if (d < -0.005f != (d == -0.005f)) {
        sunk++;
      }
    }
  }
  can_lift = 0;
  if ((unsigned char)lifted != 0 && sunk == 0) {
    can_lift = 1;
  }
  for (i = 0; i < 4; i++) {
    w = &car->wheels[i];
    d = w->contact_distance;
    if (w->in_contact == 0) {
      if (-2.0f < d && d < 0.0f != (d == 0.0f)) {
        w->in_contact = 2;
        speed_dot = (car->forward[2] * car->velocity[2] + car->forward[0] * car->velocity[0] + car->forward[1] * car->velocity[1]) / car->speed;
        if (speed_dot != 0.0f && (-0.1f < speed_dot && speed_dot < 0.1f)) {
          w->in_contact = 2;
        }
      } else {
        goto next_wheel;
      }
    } else if ((0.02f < d && can_lift) || w->contact_triangle == -1) {
      w->in_contact = 0;
      goto next_wheel;
    } else {
      w->in_contact = 1;
    }
    delta_x[i] = w->contact_normal[0] * w->contact_distance;
    points[i * 3] = g_car_8bd760[i].x - w->contact_normal[0] * w->contact_distance;
    delta_y[i] = w->contact_normal[1] * w->contact_distance;
    points[i * 3 + 1] = g_car_8bd760[i].y - w->contact_normal[1] * w->contact_distance;
    delta_z[i] = w->contact_normal[2] * w->contact_distance;
    points[i * 3 + 2] = g_car_8bd760[i].z - w->contact_normal[2] * w->contact_distance;
  next_wheel:;
  }

  count = car->wheels[0].in_contact == 1;
  sum_y = 0.0f;
  sum_z = 0.0f;
  sum_x = 0.0f;
  if (count) {
    sum_y = delta_y[0];
    sum_z = delta_z[0];
    sum_x = delta_x[0];
  }
  for (i = 1; i < 4; i++) {
    if (car->wheels[i].in_contact == 1) {
      sum_x = sum_x + delta_x[i];
      count = count + 1;
      sum_y = delta_y[i] + sum_y;
      sum_z = delta_z[i] + sum_z;
    }
  }
  if (count == 4) {
    d0 = car->wheels[0].contact_distance;
    if (d0 < 0.0f) {
      d0 = -d0;
    }
    d1 = car->wheels[1].contact_distance;
    if (d1 < 0.0f) {
      d1 = -d1;
    }
    best = d0;
    if (d0 < d1) {
      best = d1;
    }
    worst = d0 < d1;
    d2 = car->wheels[2].contact_distance;
    if (d2 < 0.0f) {
      d2 = -d2;
    }
    if (best < d2) {
      worst = 2;
      best = d2;
    }
    d3 = car->wheels[3].contact_distance;
    if (d3 < 0.0f) {
      d3 = -d3;
    }
    if (best < d3) {
      worst = 3;
    }
    sum_x = sum_x - delta_x[worst];
    count = 3;
    sum_y = sum_y - delta_y[worst];
    sum_z = sum_z - delta_z[worst];
    car->wheels[worst].in_contact = 0;
  } else if (count == 0) {
    goto settle;
  }
  inv = 1.0f / (float)count;
  matrix->pos.x = matrix->pos.x - (1.0f / (float)count) * sum_x;
  matrix->pos.y = matrix->pos.y - inv * sum_y;
  matrix->pos.z = matrix->pos.z - inv * sum_z;
  if (count == 3) {
    int a, b, c;

    a = 1;
    c = 1;
    if (car->wheels[0].in_contact == 1) {
      a = 0;
      if (car->wheels[1].in_contact != 1) {
        car->wheels[1].in_contact = 1;
        car->wheels[1].contact_normal[0] = car->wheels[0].contact_normal[0];
        car->wheels[1].contact_normal[1] = car->wheels[0].contact_normal[1];
        car->wheels[1].contact_normal[2] = car->wheels[0].contact_normal[2];
        car->wheels[1].contact_distance = car->wheels[0].contact_distance;
        car->wheels[1].contact_triangle = car->wheels[0].contact_triangle;
        b = 3;
        c = 2;
      } else if (car->wheels[2].in_contact == 1) {
        car->wheels[3].contact_normal[0] = car->wheels[2].contact_normal[0];
        car->wheels[3].contact_normal[1] = car->wheels[2].contact_normal[1];
        car->wheels[3].contact_normal[2] = car->wheels[2].contact_normal[2];
        car->wheels[3].in_contact = 1;
        car->wheels[3].contact_distance = car->wheels[2].contact_distance;
        car->wheels[3].contact_triangle = car->wheels[2].contact_triangle;
        b = 1;
        c = 2;
      } else {
        car->wheels[2].contact_normal[0] = car->wheels[3].contact_normal[0];
        car->wheels[2].contact_normal[1] = car->wheels[3].contact_normal[1];
        car->wheels[2].contact_normal[2] = car->wheels[3].contact_normal[2];
        car->wheels[2].contact_triangle = car->wheels[3].contact_triangle;
        car->wheels[2].in_contact = 1;
        car->wheels[2].contact_distance = car->wheels[3].contact_distance;
        b = 1;
        c = 3;
      }
    } else {
      car->wheels[0].contact_normal[0] = car->wheels[1].contact_normal[0];
      car->wheels[0].contact_normal[1] = car->wheels[1].contact_normal[1];
      car->wheels[0].contact_normal[2] = car->wheels[1].contact_normal[2];
      car->wheels[0].in_contact = 1;
      car->wheels[0].contact_distance = car->wheels[1].contact_distance;
      car->wheels[0].contact_triangle = car->wheels[1].contact_triangle;
      a = 1;
      b = 3;
      c = 2;
    }
    triangle_normal_r(&points[a * 3], &points[c * 3], &points[b * 3], &normal.x);
  } else if (count < 3) {
    unsigned char n;

    n = car->wheels[0].in_contact != 0;
    normal.z = 0.0f;
    normal.y = 0.0f;
    normal.x = 0.0f;
    if (n) {
      normal.x = car->wheels[0].contact_normal[0];
      normal.y = car->wheels[0].contact_normal[1];
      normal.z = car->wheels[0].contact_normal[2];
    }
    for (i = 1; i < 4; i++) {
      if (car->wheels[i].in_contact != 0) {
        n = n + 1;
        normal.x = normal.x + car->wheels[i].contact_normal[0];
        normal.y = normal.y + car->wheels[i].contact_normal[1];
        normal.z = normal.z + car->wheels[i].contact_normal[2];
      }
    }
    if (n != 0) {
      inv = 1.0f / (float)n;
      normal.x = normal.x * inv;
      normal.y = normal.y * inv;
      normal.z = inv * normal.z;
      RwV3dNormalize(&normal, &normal);
    }
    goto settle;
  }
  cross.x = normal.z * car->up[1] - normal.y * car->up[2];
  cross.y = normal.x * car->up[2] - normal.z * car->up[0];
  cross.z = normal.y * car->up[0] - normal.x * car->up[1];
  cosine = normal.z * car->up[2] + normal.y * car->up[1] + normal.x * car->up[0];
  if ((cross.x != 0.0f || cross.y != 0.0f || cross.z != 0.0f) && (cosine < 0.999f || wheels_before < 3)) {
    cosine_clamped = -1.0;
    if (!(cosine < -1.0)) {
      cosine_clamped = cosine;
      if (1.0 < cosine) {
        cosine_clamped = 1.0;
      }
    }
    angle = (float)(FUN_004b3284(cosine_clamped) * 57.29578f);
    if (limit_angle != 0 && 2.5f < angle) {
      angle = 2.5f;
    }
    saved_pos.x = matrix->pos.x;
    saved_pos.y = matrix->pos.y;
    saved_pos.z = matrix->pos.z;
    RwMatrixRotate(&rotation, &cross, angle, rwCOMBINEREPLACE);
    RwMatrixTransform(matrix, &rotation, rwCOMBINEPOSTCONCAT);
    matrix->pos.x = saved_pos.x;
    matrix->pos.y = saved_pos.y;
    matrix->pos.z = saved_pos.z;
  }
settle:
  for (i = 0; i < 4; i++) {
    w = &car->wheels[i];
    if (w->in_contact == 1) {
      if (w->surface_color == SURFACE_RESET_COLOUR) {
        w->in_contact = 0;
        push[0] = w->contact_normal[0] * 1000.0f;
        push[1] = w->contact_normal[1] * 1000.0f;
        push[2] = w->contact_normal[2] * 1000.0f;
        car->velocity[0] = push[0] + car->velocity[0];
        car->velocity[1] = push[1] + car->velocity[1];
        car->velocity[2] = push[2] + car->velocity[2];
      }
      if (w->contact_normal[1] < s_333_Lap_frames_005df368) {
        w->in_contact = 0;
        tmp_vec.x = w->contact_normal[0] * 10.0f;
        tmp_vec.y = w->contact_normal[1] * 10.0f;
        tmp_vec.z = w->contact_normal[2] * 10.0f;
        car->velocity[0] = tmp_vec.x + car->velocity[0];
        car->velocity[1] = tmp_vec.y + car->velocity[1];
        car->velocity[2] = tmp_vec.z + car->velocity[2];
        RwV3dNormalize(&tmp_vec, (RwV3d *)car->velocity);
        side_z = tmp_vec.y * w->contact_normal[0] - tmp_vec.x * w->contact_normal[1];
        push_scale = car->speed * 0.00001f;
        cross.x = (tmp_vec.z * w->contact_normal[1] - tmp_vec.y * w->contact_normal[2]) * push_scale;
        cross.y = (tmp_vec.x * w->contact_normal[2] - tmp_vec.z * w->contact_normal[0]) * push_scale;
        cross.z = side_z * push_scale;
        car->velocity_input[0] = cross.x + car->velocity_input[0];
        car->velocity_input[1] = cross.y + car->velocity_input[1];
        car->velocity_input[2] = cross.z + car->velocity_input[2];
      }
    }
  }
  car->wheels_in_contact = (float)count;
  if (car->mode == 0 && (float)count != 0.0f && ((float)count < 2.0f != ((float)count == 2.0f))) {
    unsigned char n;

    n = car->wheels[0].in_contact != 1;
    sum_z = 0.0f;
    sum_y = 0.0f;
    sum_x = 0.0f;
    if (n) {
      sum_x = WHEEL_PREVIOUS(car, 0)[0];
      sum_y = WHEEL_PREVIOUS(car, 0)[1];
      sum_z = WHEEL_PREVIOUS(car, 0)[2];
    }
    for (i = 1; i < 4; i++) {
      if (car->wheels[i].in_contact != 1) {
        sum_x = sum_x + WHEEL_PREVIOUS(car, i)[0];
        n = n + 1;
        sum_y = WHEEL_PREVIOUS(car, i)[1] + sum_y;
        sum_z = WHEEL_PREVIOUS(car, i)[2] + sum_z;
      }
    }
    inv = 1.0f / (float)n;
    tmp_vec.x = car->matrices[0xc] - (1.0f / (float)n) * sum_x;
    tmp_vec.y = car->matrices[0xd] - inv * sum_y;
    push[2] = inv * sum_z;         /* a stored (rounded) float in the original; the branch makes the compiler read it back */
    if (n == 0) {
      push[0] = 0.0f;
    }
    tmp_vec.z = car->matrices[0xe] - push[2];
    RwV3dNormalize(&tmp_vec, &tmp_vec);
    tmp_vec.x = tmp_vec.x * -20.0f;
    tmp_vec.y = 0.0f;
    tmp_vec.z = tmp_vec.z * -20.0f;
    car->velocity[0] = tmp_vec.x + car->velocity[0];
    car->velocity[1] = car->velocity[1];
    car->velocity[2] = tmp_vec.z + car->velocity[2];
    RwV3dNormalize(&tmp_vec, &tmp_vec);
    cross.x = tmp_vec.y * g_car_62ac7c - tmp_vec.z * g_car_62ac78;
    cross.y = tmp_vec.z * g_car_62ac74 - tmp_vec.x * g_car_62ac7c;
    cross.z = tmp_vec.x * g_car_62ac78 - tmp_vec.y * g_car_62ac74;
    RwV3dNormalize(&cross, &cross);
    cross.y = cross.y * -0.0625f;
    cross.z = cross.z * -0.0625f;
    car->velocity_input[0] = cross.x * -0.0625f + car->velocity_input[0];
    car->velocity_input[1] = cross.y + car->velocity_input[1];
    car->velocity_input[2] = cross.z + car->velocity_input[2];
  }
  car->wheels_in_contact = 0.0f;
  if (car->wheels[0].in_contact != 0) {
    car->wheels_in_contact = 1.0f;
  }
  if (car->wheels[1].in_contact != 0) {
    car->wheels_in_contact = car->wheels_in_contact + 1.0f;
  }
  if (car->wheels[2].in_contact != 0) {
    car->wheels_in_contact = car->wheels_in_contact + 1.0f;
  }
  if (car->wheels[3].in_contact != 0) {
    car->wheels_in_contact = car->wheels_in_contact + 1.0f;
  }
}

/* triangle_normal_r: C -> original triangle_normal (registers eax,edx,ecx,esi) */
__declspec(naked) void triangle_normal_r(float * p0, float * p1, float * p2, float * p3)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov edx, dword ptr [esp + 12]
        mov ecx, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call triangle_normal
        pop esi
        ret
    }
}

/* entry thunk at the original address of car_wheel_contact_update: register inputs (eax,edi,s0) -> natural C car_wheel_contact_update_n; ret 0 */
__declspec(naked) void car_wheel_contact_update(void)
{
    __asm {
        push dword ptr [esp + 4]
        push edi
        push eax
        call car_wheel_contact_update_n
        add esp, 12
        ret
    }
}
