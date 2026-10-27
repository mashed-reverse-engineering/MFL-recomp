// cluster-entries: rwp_collision_update
/* Installed cluster rwp_collision_update (tools/cluster_install.py from output/cluster_pilot_kg/rwp_collision_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040fdc0*/ extern int race_get_phase(void);
/*0040fe20*/ extern int __cdecl race_setup_slot_used(int slot);
/*0042a400*/ extern char * course_get_collision_bsp(void);
/*0047a5b0*/ extern void __cdecl car_add_velocity(RwV3d *velocity, int car);
/*0047abb0*/ extern void __cdecl car_get_velocity_any_mode(RwV3d *velocity, int car);
/*0048e450*/ extern int __cdecl rwp_object_get_type(int handle);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*0056b420*/ extern short __cdecl FUN_0056b420(int p0, char *object);
/*0058c8a0*/ extern char * __cdecl rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005ddb58*/ extern float k_f_1000;
/*005ddc28*/ extern float k_f_0;
/*005deb34*/ extern float k_f_300;
/*006fa7d0*/ extern unsigned int g_rw_6fa7d0;
/*006fab78*/ extern int g_rw_6fab78;
/*006fd498*/ extern int g_rw_6fd498;
/*007e7f60*/ extern int g_event_log_head;
/*007e7f64*/ extern int g_event_log_type;
/*007e8364*/ extern int g_event_log_time;
/*007e8764*/ extern int g_event_log_param;
/*007ef818*/ extern int g_last_frame_ticks;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_collision_update */

/* "large collision handling over the collision BSP for all cars" */

#define WORD_AT(base, off) (*(int *)((char *)(base) + (off)))
#define USHORT_AT(base, off) (*(unsigned short *)((char *)(base) + (off)))
#define FLOAT_AT(base, off) (*(float *)((char *)(base) + (off)))
#define EVDATA_F(f, i) (*(float *)&(f)->ev_data[i])
/* position k (3 floats) of a contact: the points are 0x28 bytes apart, the first one at +0x0c */
#define CONTACT_POINT(c, k) ((unsigned *)((char *)(c) + 0x0c + (k) * 0x28))

/* event types written to the event log by this function (event_log_dispatch plays them back) */
#define EVLOG_COLLISION_TYPE6_HIT 9
#define EVLOG_COLLISION_TYPE7_HIT 10
#define EVLOG_CAR_PUSHED 8
#define EVLOG_CAR_VS_SURFACE 0xc
#define CAR_COUNT 4
#define CAR_SIZE 0xd04
#define SURFACE_BLOCKED_A 0xffff0080 /* BSP surface codes this function ignores */
#define SURFACE_BLOCKED_B 0xffff8040

/* one contact of the collision list (the list cell holds a chain of them through +0xdc) */
typedef struct Contact {
  float normal[3];
  float point0[3];
  char _gap_18[0x16];
  unsigned short surface_a; /* +0x2e index into the BSP surface table */
  char _gap_30[2];
  unsigned short surface_b; /* +0x32 */
  char _gap_34[0x78];
  int point_count;          /* +0xac */
  float *body_data;         /* +0xb0 object data (+0 x, +4 y, +8 z, +0x20 .. a second vector) */
  char *object_a;           /* +0xb4 */
  int id_a;                 /* +0xb8 (low 16 bits are the object id, 0xffff = none) */
  char *object_b;           /* +0xbc */
  int id_b;                 /* +0xc0 */
  char _gap_c4[0x18];
  struct Contact *next;     /* +0xdc */
} Contact;

typedef struct ContactCell {
  int _gap_0;
  Contact *first;
  char _gap_8[0x20];
} ContactCell;

typedef struct ContactWorld {
  char _gap_0[0xc8];
  unsigned cell_count;
  char _gap_cc[8];
  ContactCell *cells;       /* +0xd4 */
} ContactWorld;

typedef struct BspSurface {
  int _gap_0;
  int code;
} BspSurface;

typedef struct Bsp {
  char _gap_0[0x10];
  BspSurface **surfaces;
} Bsp;

/* the stack frame of the original function, kept in one struct because the event log copies the stale parts of ev_args / ev_data too */
typedef struct Frame {
  RwV3d impulse;
  char *object_b;
  int id_b;
  char *object_a;
  int _gap_b4;
  int ev_args[8];
  float surface_x;
  float surface_y;
  float surface_z;
  unsigned cell_index;
  unsigned car_offset;
  RwV3d car_velocity;
  Bsp *bsp;
  unsigned ev_data[16];
  char *slot_object[CAR_COUNT];
  int slot_used[CAR_COUNT];
  RwV3d direction;
} Frame;

/* appends one record to the 256 entry event log, unless the race is in phase 1..5 */
static void evlog_push(int type, int param, Frame *f)
{
  int phase;
  int ticks;
  int k;

  phase = race_get_phase();
  ticks = g_last_frame_ticks;
  if ((phase != 1) && (phase != 2) && (phase != 3) && (phase != 4) && (phase != 5)) {
    (&g_event_log_type)[g_event_log_head] = type;
    (&g_event_log_time)[g_event_log_head] = ticks;
    (&g_event_log_param)[g_event_log_head] = param;
    for (k = 0; k < 8; k++) {
      (&g_event_log_args[g_event_log_head].arg0)[k] = f->ev_args[k];
    }
    for (k = 0; k < 16; k++) {
      ((unsigned *)&g_event_log_data[g_event_log_head])[k] = f->ev_data[k];
    }
    g_event_log_head = g_event_log_head + 1;
    if (0xff < g_event_log_head) {
      g_event_log_head = 0;
    }
  }
}

/* copies the contact points into ev_data (3 words each) */
static void contact_points_to_event(Contact *c, int count, Frame *f)
{
  int k;
  unsigned *src;

  for (k = 0; k < count; k++) {
    src = CONTACT_POINT(c, k);
    f->ev_data[k * 3] = src[0];
    f->ev_data[k * 3 + 1] = src[1];
    f->ev_data[k * 3 + 2] = src[2];
  }
}

/* index + 1 of the last active car whose slot object is `object`, 0 when there is none */
static int find_active_car(Frame *f, char *object)
{
  int i;
  int found = 0;

  for (i = 0; i < CAR_COUNT; i++) {
    if ((player_car[i].rwp_object != 0) && (f->slot_used[i] != 0) && (object == f->slot_object[i])) {
      found = i + 1;
    }
  }
  return found;
}

/* index of the last used slot whose object is `object`, -1 when there is none */
static int find_slot(Frame *f, char *object)
{
  int i;
  int found = -1;

  for (i = 0; i < CAR_COUNT; i++) {
    if ((f->slot_used[i] != 0) && (object == f->slot_object[i])) {
      found = i;
    }
  }
  return found;
}

/* a car (found + 1 = slot) touches the BSP: logs the hit with the contact points, the position in 1/1000 and the surface vector of the car's physics object */
static void car_hits_surface(Frame *f, Contact *c, int found)
{
  int *body;
  int *hull;
  char *table;
  int offset;

  contact_points_to_event(c, c->point_count, f);
  f->ev_args[2] = (int)(c->normal[0] * k_f_1000);
  f->ev_args[3] = (int)(c->normal[1] * k_f_1000);
  f->ev_args[4] = (int)(c->normal[2] * k_f_1000);
  f->ev_args[1] = found - 1;
  f->ev_args[0] = c->point_count;
  body = *(int **)(WORD_AT(f->slot_object[found - 1], 0x10) + 4);
  if (body != 0) {
    table = *(char **)(WORD_AT(WORD_AT(body, 0), 0x10) + 8);
    offset = body[1] * 0x20;
    f->surface_x = FLOAT_AT(table, offset);
    f->surface_y = FLOAT_AT(table, offset + 4);
    f->surface_z = FLOAT_AT(table, offset + 8);
    f->impulse.x = f->surface_x * k_f_300;
    f->impulse.y = f->surface_y * k_f_300;
    f->impulse.z = f->surface_z * k_f_300;
    EVDATA_F(f, 12) = f->impulse.x;
    EVDATA_F(f, 13) = f->impulse.y;
    EVDATA_F(f, 14) = f->impulse.z;
    evlog_push(EVLOG_CAR_VS_SURFACE, found + 0xfeff, f);
  }
}

/* the other object of a car contact is a kind 6 object: log its id */
static void object_kind6_hit(Frame *f, char *object)
{
  int id;

  if (FUN_0056b420(WORD_AT(object, 0x24), object) == 6) {
    id = USHORT_AT(object, 0x20);
    *(char **)&f->ev_args[1] = object;
    f->ev_args[0] = id;
    evlog_push(EVLOG_COLLISION_TYPE6_HIT, id, f);
  }
}

/* the car in slot `i` touches `other` (a non car object): pushes the car along the contact, logs the push and adds the (damped) velocity */
static void car_pushed(Frame *f, Contact *c, int i, char *other, int body_offset)
{
  unsigned id;
  char *owner;
  int type;
  float scale;

  if (find_slot(f, other) != -1) {
    return;
  }
  f->impulse.x = FLOAT_AT(c->body_data, body_offset) + FLOAT_AT(c->body_data, body_offset);
  f->ev_args[2] = 0;
  f->ev_data[1] = *(unsigned *)&player_car[i].speed;
  f->impulse.y = FLOAT_AT(c->body_data, body_offset + 4) + FLOAT_AT(c->body_data, body_offset + 4);
  EVDATA_F(f, 0) = 1.0f;
  f->impulse.z = FLOAT_AT(c->body_data, body_offset + 8) + FLOAT_AT(c->body_data, body_offset + 8);
  id = USHORT_AT(other, 0x20);
  f->ev_args[0] = i;
  if (id < g_rw_6fa7d0) {
    f->ev_args[1] = id;
    owner = rwp_owner_object_get((&g_rw_6fab78)[id]);
    if ((owner != 0) && (WORD_AT(WORD_AT(owner, 0x10), 4) != 0)) {
      type = rwp_object_get_type(id);
      f->ev_args[2] = g_rwp_objects[type].material;
      f->ev_data[0] = *(unsigned *)&g_rwp_objects[type].property_d;
    }
    f->ev_data[9] = ((unsigned *)c->point0)[0];
    f->ev_data[10] = ((unsigned *)c->point0)[1];
    f->ev_data[11] = ((unsigned *)c->point0)[2];
    f->ev_data[12] = ((unsigned *)c->normal)[0];
    f->ev_data[13] = ((unsigned *)c->normal)[1];
    f->ev_data[14] = ((unsigned *)c->normal)[2];
    evlog_push(EVLOG_CAR_PUSHED, i + 0xff00, f);
  }
  car_get_velocity_any_mode(&f->car_velocity, i);
  if ((f->impulse.x != k_f_0) && (f->impulse.y != k_f_0) && (f->impulse.z != k_f_0) &&
      (f->car_velocity.x != k_f_0) && (f->car_velocity.y != k_f_0) && (f->car_velocity.z != k_f_0)) {
    RwV3dNormalize(&f->direction, &f->impulse);
    RwV3dNormalize(&f->car_velocity, &f->car_velocity);
    scale = float_1_0 - (f->direction.z * f->car_velocity.z + f->direction.x * f->car_velocity.x + f->direction.y * f->car_velocity.y);
    if (scale > float_1_0) {
      scale = float_1_0;
    }
    else if (scale < k_f_0) {
      scale = k_f_0;
    }
    f->impulse.x = f->impulse.x * scale;
    f->impulse.y = f->impulse.y * scale;
    f->impulse.z = scale * f->impulse.z;
  }
  car_add_velocity(&f->impulse, i);
}

/* a car (found = slot + 1) is the one object of a contact whose other side is `object`: log the contact points / the position, and the touched object */
static void contact_car_world(Frame *f, Contact *c, char *car_side_other, int surface_index)
{
  int found;
  int code;

  found = find_active_car(f, car_side_other);
  code = f->bsp->surfaces[surface_index]->code;
  if ((code != SURFACE_BLOCKED_A) && (code != SURFACE_BLOCKED_B) && (found != 0)) {
    car_hits_surface(f, c, found);
  }
  object_kind6_hit(f, car_side_other);
}

/* contact between two objects that both have ids: kind 7 objects touching a car are logged, then every car among the two is pushed away by the other */
static void contact_objects(Frame *f, Contact *c)
{
  int found;
  int kind;
  int i;
  unsigned id;

  if (FUN_0056b420(WORD_AT(f->object_a, 0x24), f->object_a) == 7) {
    f->ev_args[0] = USHORT_AT(f->object_a, 0x20);
    *(char **)&f->ev_args[1] = f->object_a;
    found = find_slot(f, f->object_b);
    if (found != -1) {
      f->ev_args[2] = c->point_count;
      contact_points_to_event(c, c->point_count, f);
      EVDATA_F(f, 12) = -c->normal[0];
      EVDATA_F(f, 13) = -c->normal[1];
      EVDATA_F(f, 14) = -c->normal[2];
      evlog_push(EVLOG_COLLISION_TYPE7_HIT, found + 0xff00, f);
    }
  }
  else if (FUN_0056b420(WORD_AT(f->object_b, 0x24), f->object_b) == 7) {
    f->ev_args[0] = USHORT_AT(f->object_b, 0x20);
    *(char **)&f->ev_args[1] = f->object_b;
    found = find_slot(f, f->object_a);
    if (found != -1) {
      f->ev_args[2] = c->point_count;
      contact_points_to_event(c, c->point_count, f);
      f->ev_data[12] = ((unsigned *)c->normal)[0];
      f->ev_data[13] = ((unsigned *)c->normal)[1];
      f->ev_data[14] = ((unsigned *)c->normal)[2];
      evlog_push(EVLOG_COLLISION_TYPE7_HIT, found + 0xff00, f);
    }
  }
  f->car_offset = 0;
  i = 0;
  do {
    if (f->slot_used[i] != 0) {
      kind = (f->object_a == f->slot_object[i]);
      if (f->object_b == f->slot_object[i]) {
        kind = 2;
      }
      if (kind != 0) {
        if (kind == 1) {
          car_pushed(f, c, i, f->object_b, 0);
        }
        else {
          car_pushed(f, c, i, f->object_a, 0x20);
        }
      }
    }
    f->car_offset = f->car_offset + CAR_SIZE;
    i = i + 1;
  } while (f->car_offset < CAR_COUNT * CAR_SIZE);
}

void rwp_collision_update(ContactWorld *world)

{
  Frame f;
  Contact *c;
  int i;

  f.bsp = (Bsp *)course_get_collision_bsp();
  for (i = 0; i < CAR_COUNT; i++) {
    f.slot_object[i] = rwp_owner_object_get((&g_rw_6fd498)[i]);
    f.slot_used[i] = race_setup_slot_used(i);
  }
  f.cell_index = 0;
  if (world->cell_count != 0) {
    do {
      for (c = world->cells[f.cell_index].first; c != 0; c = c->next) {
        f.object_a = c->object_a;
        f.object_b = c->object_b;
        f.id_b = c->id_b;
        if ((short)c->id_a == -1) {
          if ((short)f.id_b != -1) {
            contact_car_world(&f, c, f.object_b, c->surface_a);
          }
        }
        else if ((short)f.id_b == -1) {
          contact_car_world(&f, c, f.object_a, c->surface_b);
        }
        else {
          contact_objects(&f, c);
        }
      }
      f.cell_index = f.cell_index + 1;
    } while (f.cell_index < world->cell_count);
  }
  return;
}

