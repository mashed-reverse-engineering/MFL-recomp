// cluster-entries: rwp_cars_update
/* Installed cluster rwp_cars_update (tools/cluster_install.py from output/cluster_pilot_kg/rwp_cars_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count(void);
/*0040fdc0*/ extern int race_get_phase(void);
/*0042a3f0*/ extern int course_get_rpworld(void);
/*00426f50*/ extern double atan2_degrees(float x, float z);
/*0047abb0*/ extern void __cdecl car_get_velocity_any_mode(float *velocity, int car);
/*0047b520*/ extern void __cdecl car_current_matrix(RwMatrix **matrix, int car);
/*0047b550*/ extern void __cdecl car_set_matrices_from_floats(int car, RwMatrix *matrix);
/*0047b590*/ extern void __cdecl car_get_forward(float *forward, int car);
/*00490480*/ extern int __cdecl rwp_world_step_forward(int world, float dt);
/*0048e960*/ extern void __cdecl rwp_cars_setup_bodies_from_bounds(int world, int rpworld);
/*004b3284*/ extern double __cdecl FUN_004b3284(double x);
/*004d0f70*/ extern void __cdecl RwFrameUpdateObjects(void *frame);
/*004d3cc0*/ extern void __cdecl RwMatrixUpdate(RwMatrix *matrix);
/*004d56d0*/ extern void __cdecl RwV3dTransformVectors(RwV3d *out, RwV3d *in, int count, RwMatrix *matrix);
/*005543e0*/ extern int __cdecl FUN_005543e0(int p0);
/*0056b1d0*/ extern void __cdecl FUN_0056b1d0(int p0, char *object, int p2);
/*0056e5b0*/ extern void __cdecl FUN_0056e5b0(char *object, int p1);
/*00573e50*/ extern void __cdecl FUN_00573e50(int p0);
/*0058c8a0*/ extern char * __cdecl rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddaec*/ extern float k_f_57_29578;
/*005ddafc*/ extern float k_f_0_05;
/*005ddea0*/ extern float k_f_20;
/*005e0454*/ extern float k_f_2_0en4;
/*005e64a4*/ extern float k_f_0;
/*0062ac74*/ extern RwV3d g_car_62ac74;
/*0062ac80*/ extern RwV3d g_weapon_62ac80;
/*006299fc*/ extern float g_rwp_cars_6299fc;
/*006fd498*/ extern int g_rw_6fd498;
/*006fe768*/ extern int g_rwp_cars_6fe768;
/*00701c9c*/ extern int g_rw_701c9c;
/*00771a20*/ extern int g_os_771a20;
/*00801b14*/ extern int g_rw_801b14;
/*00888620*/ extern int g_car_888620;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_cars_update */

#define CAR_COUNT 4
#define CAR_STRUCT_SIZE 0xd04
#define PLAYER_BODY_Y_PARKED -10000.0f /* bodies of absent players are moved far below the course */
#define RACE_CAR_WORLD_PLACEHOLDER 0x7b /* g_car_888620 value that asks for the car bodies to be created */

#define WORD_AT(base, off) (*(int *)((char *)(base) + (off)))

/* the physics body of a car: owner object +0x10 -> { +4 -> { hull, index }, +8 -> the RwMatrix of the body } */
#define BODY_DATA(owner) WORD_AT(owner, 0x10)
#define BODY_HULL(owner) ((int *)WORD_AT(BODY_DATA(owner), 4))
#define BODY_MATRIX(owner) ((RwMatrix *)WORD_AT(BODY_DATA(owner), 8))
/* the RwFrame of the car's owner handle */
#define OWNER_FRAME(i) ((void *)WORD_AT((&g_rw_6fd498)[i], 4))

/* the stack frame of the original function: car_get_velocity_any_mode and car_get_forward share v[], and the harness (and the original's stale words) see the neighbours of every out parameter */
typedef struct Frame {
  float v[3];
  RwV3d impulse;
  float rec_x, rec_y, rec_z;
  union {
    char *owner;     /* the owner object of the car being updated, later the result of the world step */
    int step_result;
  };
  float heading;
  struct Car *car;
  RwV3d offset;
  RwV3d axis;
  float cross_x, cross_y, cross_z;
  RwMatrix *current;
  RwV3d offset2;
  float pos_x, pos_y, pos_z;
  RwMatrix copy;
} Frame;

/* "keeps the physics bodies of the player cars in step with the car state and advances the physics world; returns whether the step succeeded" */
int rwp_cars_update(void)

{
  Frame f;
  int i;
  int phase;
  int world;
  int *hull;
  RwMatrix *body;
  float *record;
  double angle;
  double dot;
  float step;

  if (g_rw_701c9c == 0) {
    return 0;
  }
  phase = race_get_phase();
  if (phase != 6) {
    g_rwp_cars_6fe768 = 0;
  }
  if (g_car_888620 == RACE_CAR_WORLD_PLACEHOLDER) {
    world = course_get_rpworld();
    if ((g_os_771a20 == 1) || ((4 < g_os_771a20) && (g_os_771a20 < 7))) {
      FUN_00573e50(g_rw_801b14);
    }
    rwp_cars_setup_bodies_from_bounds(g_rw_701c9c, world);
    g_car_888620 = 0;
  }
  if (k_f_0_05 < g_rwp_cars_6299fc) {
    g_rwp_cars_6299fc = 0.05f;
  }
  g_rwp_cars_6fe768 = g_rwp_cars_6fe768 + 1;
  i = 0;
  f.car = player_car;
  do {
    if ((f.car->rwp_object == 0) && (i < get_player_count())) {
      car_current_matrix(&f.current, i);
      f.owner = rwp_owner_object_get((&g_rw_6fd498)[i]);
      hull = BODY_HULL(f.owner);
      body = BODY_MATRIX(f.owner);
      if (hull != 0) {
        car_get_velocity_any_mode(f.v, i);
        f.v[0] = f.v[0] * k_f_2_0en4;
        f.v[1] = f.v[1] * k_f_2_0en4;
        f.v[2] = f.v[2] * k_f_2_0en4;
        f.v[0] = f.v[0] + f.current->pos.x;
        f.v[1] = f.v[1] + f.current->pos.y;
        f.v[2] = f.v[2] + f.current->pos.z;
        f.offset.x = f.car->rwp_body_offset[0];
        f.offset.y = f.car->rwp_body_offset[1];
        f.offset.z = f.car->rwp_body_offset[2];
        RwV3dTransformVectors(&f.offset, &f.offset, 1, body);
        record = (float *)(WORD_AT(WORD_AT(*hull, 0x10), 8) + hull[1] * 0x20);
        f.rec_z = f.v[2] - (f.offset.z + body->pos.z); /* the original rounds the z difference to float before the scale */
        f.rec_x = (f.v[0] - (f.offset.x + body->pos.x)) * k_f_20;
        f.rec_y = (f.v[1] - (f.offset.y + body->pos.y)) * k_f_20;
        record[0] = f.rec_x;
        f.rec_z = f.rec_z * k_f_20;
        record[1] = f.rec_y;
        record[2] = f.rec_z;
        car_get_forward(f.v, i);
        f.heading = (float)atan2_degrees(f.v[0], f.v[2]);
        RwV3dTransformVectors(&f.offset, &g_weapon_62ac80, 1, body);
        angle = atan2_degrees(f.offset.x, f.offset.z);
        angle = (angle - f.heading) + k_f_180;
        if (angle < k_f_0) {
          do {
            angle = angle + k_f_360;
          } while (angle < k_f_0);
        }
        if (k_f_180 < angle) {
          angle = -(k_f_360 - angle);
        }
        if (angle <= k_f_0) {
          angle = -(angle + k_f_180);
        }
        else {
          angle = k_f_180 - angle;
        }
        f.heading = (float)angle;
        f.rec_y = (float)angle;
        f.rec_x = 0.0f;
        f.rec_z = 0.0f;
        RwV3dTransformVectors(&f.axis, &g_car_62ac74, 1, body);
        f.cross_x = f.axis.y * f.car->up[2] - f.axis.z * f.car->up[1];
        f.cross_y = f.axis.z * f.car->up[0] - f.axis.x * f.car->up[2];
        f.cross_z = f.axis.x * f.car->up[1] - f.axis.y * f.car->up[0];
        dot = f.axis.y * f.car->up[1];
        dot = dot + f.axis.x * f.car->up[0];
        dot = dot + f.axis.z * f.car->up[2];
        if (((f.cross_x != k_f_0) || (f.cross_y != k_f_0) || (f.cross_z != k_f_0)) && (dot < float_1_0)) {
          if (dot < k_f_n1) {
            dot = k_f_n1;
          }
          else if (dot > float_1_0) {
            dot = float_1_0;
          }
          angle = FUN_004b3284(dot) * k_f_57_29578;
          *(int *)&f.pos_x = *(int *)&body->pos.x;
          *(int *)&f.pos_z = *(int *)&body->pos.z;
          f.rec_x = (float)(f.cross_x * angle);
          *(int *)&body->pos.x = *(int *)&f.pos_x;
          *(int *)&body->pos.y = *(int *)&body->pos.y;
          *(int *)&body->pos.z = *(int *)&f.pos_z;
          f.rec_y = (float)(f.cross_y * angle + f.heading);
          f.rec_z = (float)(angle * f.cross_z);
        }
        record = (float *)(WORD_AT(WORD_AT(*hull, 0x10), 8) + 0x10 + hull[1] * 0x20);
        record[0] = f.rec_x;
        record[1] = f.rec_y;
        record[2] = f.rec_z;
        RwMatrixUpdate(body);
        RwFrameUpdateObjects(OWNER_FRAME(i));
        FUN_0056e5b0(f.owner, 0);
        FUN_0056b1d0(WORD_AT(f.owner, 0x24), f.owner, 1);
      }
      phase = race_get_phase();
      if ((phase == 3) || (phase == 5)) {
        f.impulse = body->pos;
        *body = *f.current;
        f.offset2.x = f.car->rwp_body_offset[0];
        f.offset2.y = f.car->rwp_body_offset[1];
        f.offset2.z = f.car->rwp_body_offset[2];
        RwV3dTransformVectors(&f.offset2, &f.offset2, 1, body);
        body->pos.x = body->pos.x - f.offset2.x;
        body->pos.y = body->pos.y - f.offset2.y;
        body->pos.z = body->pos.z - f.offset2.z;
        RwMatrixUpdate(body);
        RwFrameUpdateObjects(OWNER_FRAME(i));
        FUN_0056e5b0(f.owner, 0);
        FUN_0056b1d0(WORD_AT(f.owner, 0x24), f.owner, 1);
      }
    }
    if (get_player_count() <= i) {
      f.owner = rwp_owner_object_get((&g_rw_6fd498)[i]);
      body = BODY_MATRIX(f.owner);
      body->pos.x = 0.0f;
      body->pos.y = PLAYER_BODY_Y_PARKED;
      body->pos.z = 0.0f;
      RwMatrixUpdate(body);
      RwFrameUpdateObjects(OWNER_FRAME(i));
      FUN_0056e5b0(f.owner, 0);
      FUN_0056b1d0(WORD_AT(f.owner, 0x24), f.owner, 0);
    }
    f.car = (struct Car *)((char *)f.car + CAR_STRUCT_SIZE);
    i = i + 1;
  } while (i < CAR_COUNT);
  step = g_rwp_cars_6299fc;
  world = FUN_005543e0(g_rw_701c9c);
  f.step_result = rwp_world_step_forward(world, step);
  f.car = player_car;
  i = 0;
  do {
    if (f.car->rwp_object != 0) {
      f.copy = *(RwMatrix *)((char *)OWNER_FRAME(i) + 0x10);
      f.impulse.x = f.car->rwp_body_offset[0] * k_f_n1;
      f.impulse.y = f.car->rwp_body_offset[1] * k_f_n1;
      f.impulse.z = f.car->rwp_body_offset[2] * k_f_n1;
      RwV3dTransformVectors(&f.impulse, &f.impulse, 1, &f.copy);
      f.copy.pos.x = f.copy.pos.x - f.impulse.x;
      f.copy.pos.y = f.copy.pos.y - f.impulse.y;
      f.copy.pos.z = f.copy.pos.z - f.impulse.z;
      car_set_matrices_from_floats(i, &f.copy);
      rwp_owner_object_get((&g_rw_6fd498)[i]);
    }
    f.car = (struct Car *)((char *)f.car + CAR_STRUCT_SIZE);
    i = i + 1;
  } while (i < CAR_COUNT);
  return f.step_result != 0;
}

