// cluster-entries: rwp_car_collision_response
/* Installed cluster rwp_car_collision_response (tools/cluster_install.py from output/cluster_pilot_kg/rwp_car_collision_response.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/*0040fdc0*/ extern int race_get_phase(void);
/*0042a400*/ extern char * course_get_collision_bsp(void);
/*0047ac30*/ extern void __cdecl car_get_mode(int car, int *mode, int *out);
/*0047b520*/ extern void __cdecl car_current_matrix(char **matrix, int car);
/*0058c8a0*/ extern char * __cdecl rwp_owner_object_get(int p0);
/*006fd498*/ extern int g_rw_6fd498;
/*007e7f60*/ extern int g_event_log_head;
/*007e7f64*/ extern int g_event_log_type;
/*007e8364*/ extern int g_event_log_time;
/*007e8764*/ extern int g_event_log_param;
/*007ef7f0*/ extern int g_tick_count;
/*007ef818*/ extern int g_last_frame_ticks;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_car_collision_response */

/* "collision BSP queries for the car mode" */

#define CAR_COUNT 4
#define CAR_STRUCT_SIZE 0xd04
#define EVLOG_CAR_CONTACT 6
/* BSP surface codes that a car contact reacts to */
#define SURFACE_CAR_BLOCK 0xff010101
#define SURFACE_CAR_SOFT 0xff0000ff
#define SURFACE_CAR_PICKUP 0xffff0080
#define CONTACT_COUNT_FREEZE 0x32 /* data[0] above this counts as a hard hit */

/* the stack frame of the original function: the event log records copy four stale words behind the owner table (ev[4..7]) and the car state out parameter */
typedef struct Frame {
  int removed;
  int car_slot;          /* index + 1 of the car that owns the contact, 0 when none */
  int *prev_link;        /* the link that points to the current contact */
  int mode;              /* car_get_mode */
  struct Car *car;
  unsigned cell;
  int ev[8];             /* owner objects of the four cars, then four words of stale stack */
  char *bsp;
  char *point_surfaces;  /* the surface ids of the current contact point */
  int car_not_in_rwp;
  int side;              /* 1: the car is the second object of the contact, 2: the first */
  unsigned point;
  char *matrix;
  int data[21];          /* [0] car_get_mode result, [1..4] "already counted" flags per car, [5..20] event data (words) */
} Frame;

/* appends one record to the 256 entry event log, unless the race is in phase 1..5 */
static void evlog_contact(Frame *f, int code)
{
  int phase;
  int ticks;
  int k;
  struct EventLogArgs *args;

  phase = race_get_phase();
  ticks = g_last_frame_ticks;
  if ((phase != 1) && (phase != 2) && (phase != 3) && (phase != 4) && (phase != 5)) {
    (&g_event_log_type)[g_event_log_head] = EVLOG_CAR_CONTACT;
    (&g_event_log_time)[g_event_log_head] = ticks;
    (&g_event_log_param)[g_event_log_head] = f->car_slot + 0xfeff;
    args = &g_event_log_args[g_event_log_head];
    args->arg0 = code;
    args->arg1 = f->ev[1];
    args->arg2 = f->ev[2];
    args->w0c = f->ev[3];
    args->w10 = f->ev[4];
    args->w14 = f->ev[5];
    args->w18 = f->ev[6];
    args->w1c = f->ev[7];
    for (k = 0; k < 16; k++) {
      ((int *)&g_event_log_data[g_event_log_head])[k] = f->data[k + 5];
    }
    g_event_log_head = g_event_log_head + 1;
    if (0xffu < (unsigned int)g_event_log_head) {      /* unsigned index in the original (ja) */
      g_event_log_head = 0;
    }
  }
}

void rwp_car_collision_response(char *param_1)

{
  Frame f;
  int contact;
  int k;
  int code;
  int other;
  int *key;
  int i;

  f.data[1] = 0;
  f.data[2] = 0;
  f.data[3] = 0;
  f.data[4] = 0;
  for (k = 0; k < CAR_COUNT; k++) {
    f.ev[k] = (int)rwp_owner_object_get((&g_rw_6fd498)[k]);
  }
  f.bsp = course_get_collision_bsp();
  f.cell = 0;
  if (*(int *)(param_1 + 200) != 0) {
    do {
      f.prev_link = (int *)(*(char **)(param_1 + 0xd4) + 4 + f.cell * 0x28);
      for (contact = *f.prev_link; contact != 0; contact = *(int *)(contact + 0xdc)) {
        other = *(int *)(contact + 0xb4);
        f.removed = 0;
        if ((short)*(int *)(contact + 0xb8) == -1) {
          f.side = 1;
          other = *(int *)(contact + 0xbc);
        }
        else if ((short)*(int *)(contact + 0xc0) == -1) {
          f.side = 2;
        }
        else {
          f.prev_link = (int *)(contact + 0xdc);
          continue;
        }
        f.car_slot = 0;
        f.car_not_in_rwp = 0;
        f.point = 0;
        for (k = 0; k < CAR_COUNT; k++) {
          if ((player_car[k].active != 0) && (other == f.ev[k])) {
            f.car_slot = k + 1;
            f.car = &player_car[k];
            if (player_car[k].rwp_object == 0) {
              f.car_not_in_rwp = 1;
            }
            car_get_mode(k, &f.mode, f.data);
          }
        }
        f.point = 0;
        if (*(int *)(contact + 0xac) != 0) {
          f.point_surfaces = (char *)(contact + 0x30);
          do {
            if (f.removed) break;
            if ((f.car_slot != 0) && (f.data[f.car_slot] == 0)) {
              *(int *)((char *)f.car + 0xc) = *(int *)((char *)f.car + 0xc) + g_tick_count * 2;
              f.data[f.car_slot] = 1;
            }
            if (f.side == 1) {
              i = *(unsigned short *)(f.point_surfaces - 2);
            }
            else {
              i = *(unsigned short *)(f.point_surfaces + 2);
            }
            code = *(int *)(*(char **)(*(int *)(f.bsp + 0x10) + i * 4) + 4);
            if (code == (int)SURFACE_CAR_BLOCK) {
              *f.prev_link = *(int *)(contact + 0xdc);
              f.removed = 1;
              if ((f.car_slot != 0) && (f.mode == 0)) {
                evlog_contact(&f, code);
              }
            }
            else if (f.car_slot != 0) {
              if (code == (int)SURFACE_CAR_SOFT) {
                if ((f.mode == 0) || (f.mode == 1)) {
                  evlog_contact(&f, SURFACE_CAR_SOFT);
                }
              }
              else if (code == (int)SURFACE_CAR_PICKUP) {
                if ((f.mode == 0) || (f.mode == 1)) {
                  car_current_matrix(&f.matrix, f.car_slot - 1);
                  f.data[5] = *(int *)(f.matrix + 0x30);
                  f.data[6] = *(int *)(f.matrix + 0x34);
                  f.data[7] = *(int *)(f.matrix + 0x38);
                  evlog_contact(&f, SURFACE_CAR_PICKUP);
                }
              }
              else if ((f.mode == 1) && (CONTACT_COUNT_FREEZE < f.data[0])) {
                evlog_contact(&f, SURFACE_CAR_SOFT);
              }
            }
            if (f.car_slot != 0) {
              key = (int *)((char *)f.car + 0xbfc);
              for (i = 0; i < 32; i++) {
                if ((key[0x20 + i] != 0) && (code == key[i])) {
                  *f.prev_link = *(int *)(contact + 0xdc);
                  f.removed = 1;
                }
              }
            }
            f.point = f.point + 1;
            f.point_surfaces = f.point_surfaces + 0x28;
          } while (f.point < *(unsigned *)(contact + 0xac));
          if (f.removed) {
            continue;
          }
        }
        if (f.car_not_in_rwp == 0) {
          f.prev_link = (int *)(contact + 0xdc);
        }
        else {
          *f.prev_link = *(int *)(contact + 0xdc);
        }
      }
      f.cell = f.cell + 1;
    } while (f.cell < *(unsigned *)(param_1 + 200));
  }
  return;
}

