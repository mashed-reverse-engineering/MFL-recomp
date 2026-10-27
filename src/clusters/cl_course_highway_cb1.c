// cluster-entries: course_highway_cb1
/* Installed cluster course_highway_cb1 (tools/cluster_install.py from output/cluster_pilot_ql2/course_highway_cb1.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*00459cd0*/ extern void course_object_distance_table_build(void);
/*00453c20*/ extern void course_highway_cb1_sub_453c20(RwV3d *point);
/*00454150*/ extern void course_highway_cb1_sub_454150(void *matrix);
/*00453bf0*/ extern void course_highway_cb1_sub_453bf0(void);
/*00453d00*/ extern void course_highway_cb1_sub_453d00(void *matrix);
/*004c6600*/ extern void __cdecl FUN_004c6600(void *bits, int count);
/*004c45e0*/ extern double __cdecl FUN_004c45e0(float low, float high);
/*004d5410*/ extern float _rwSqrt(float x);
/*0040fdc0*/ extern int race_get_phase(void);
/*0042b110*/ extern int current_course_id(void);
/*00487be0*/ extern int effect_slot_alloc(void);
/*00487e80*/ extern void __cdecl effect_pool_set_field_14(int effect, int value);
/*00487ea0*/ extern void __cdecl effect_pool_set_field_1c(int effect, float value);
/*00487c60*/ extern int __cdecl effect_trail_add_point(int effect, RwV3d *point, int flag, float width, float fade);
/*0048e720*/ extern void __cdecl rwp_object_set_matrix(int handle, RwMatrix *matrix);
/*005685e0*/ extern int SUB_005685e0(void *object);
/*004d1460*/ extern struct RwFrame *RwFrameTranslate(struct RwFrame *frame, RwV3d *translation, enum RwOpCombineType combine);
/*004d1640*/ extern struct RwFrame *RwFrameRotate(struct RwFrame *frame, RwV3d *axis, RwReal angle, enum RwOpCombineType combine);

#define HIGHWAY_CARS 80                       /* traffic cars of the highway course */
#define HIGHWAY_TRAILS 4                      /* tail light trails per car */
#define HIGHWAY_LANES_PER_SIDE 4              /* rows of the per side tables */
#define HIGHWAY_SIDE_FORWARD 1                /* side 1 drives towards +z, side 0 towards -z */
#define HIGHWAY_SIDE_BACKWARD 0
#define RACE_PHASE_RACING_VALUE 6
#define COURSE_ID_HIGHWAY_NIGHT 0xc

#define RAMP_SIDE_SCALE 10.0f                 /* weight of the value delta in the rise of the forward ramp */
#define BLEND_ONE 1.0f
#define BLEND_ZERO 0.0f
#define TRAIL_BLEND_DROP -0.015f              /* trails are emitted while the blend falls faster than this per frame */
#define TRAIL_HEIGHT 0.01f
#define TRAIL_SIDE_OFFSET 0.2f
#define TRAIL_POINT_WIDTH 0.08f
#define TRAIL_POINT_FADE 2.0f
#define TRAIL_EFFECT_LIFETIME 10.0f
#define TRAIL_TICK_MASK 4                     /* a trail point every 8 ticks for 4 ticks */
#define EXHAUST_SPREAD_X_LOW 0.2f
#define EXHAUST_SPREAD_X_HIGH 0.3f
#define EXHAUST_SPREAD_Y_LOW 0.0f
#define EXHAUST_SPREAD_Y_HIGH 0.1f
#define EXHAUST_SPREAD_Z_LOW -0.1f
#define EXHAUST_SPREAD_Z_HIGH 0.1f
#define LANE_FAR_Z 146.0f                     /* cars wrap around between these z values */
#define LANE_NEAR_Z -122.0f
#define START_FORWARD_Z -74.0f                /* a parked car starts when the player is past this z */
#define START_BACKWARD_Z 126.0f
#define WHEEL_SPIN_RATE 25.2f
#define FULL_TURN 360.0f
#define HALF_TURN 180.0f
#define SKY_TURN_Y 45.0f
#define SKY_TURN_X -5.0f

/* Frame object (target record): +0xa0 holds the object whose RenderWare frame carries the position */
typedef struct HighwayFrameRef {
  int unknown_00;
  struct RwFrame *frame;                      /* +4: the matrix (position at +0x40 of the frame) */
} HighwayFrameRef;

typedef struct HighwayTarget {
  char unknown_00[0xa0];
  HighwayFrameRef *frame_ref;
} HighwayTarget;

typedef struct HighwayTrail {                 /* 0x18 bytes */
  float offset[3];                            /* offset of the light from the car */
  HighwayFrameRef *lod_ref;                   /* wheel / LOD object of this slot */
  int effect[2];                              /* the two trail effects (left / right) */
} HighwayTrail;

/* One highway traffic car (0xfc bytes) */
typedef struct HighwayCar {
  HighwayTrail trail[HIGHWAY_TRAILS];         /* 0x00 */
  float angle;                                /* 0x60 wheel angle in degrees */
  int trail_count;                            /* 0x64 */
  char unknown_68[0x18];
  RwV3d position;                             /* 0x80 */
  char unknown_8c[0x14];
  HighwayFrameRef *body;                      /* 0xa0 */
  int unknown_a4[2];
  int lane;                                   /* 0xac */
  int side;                                   /* 0xb0 */
  int group;                                  /* 0xb4 */
  int unknown_b8;
  HighwayTarget *target;                      /* 0xbc the player vehicle */
  float value;                                /* 0xc0 z distance to the target in the direction of the car */
  float value_prev;
  float value_delta;
  float blend;                                /* 0xcc 0..1: how much the car is in play */
  float blend_prev;
  float blend_delta;
  float rise;                                 /* 0xd8 */
  float rise_prev;
  float rise_delta;
  float fall;                                 /* 0xe4 */
  float fall_prev;
  float fall_delta;
  int moving;                                 /* 0xf0 */
  struct { char unknown_00[0x18]; int handle; } *physics;   /* 0xf4 physics object slot, set by sub_459f70 */
  int visible;                                /* 0xf8 */
} HighwayCar;

/* The frame of the original is one block of 23 words that the phases reuse: this is the layout (word offsets in comments). */
typedef struct HighwayFrame {
  float value;                                /* w0: z distance of the car */
  float distance;                             /* w1: distance out value of sub_459dd0 */
  float ramp;                                 /* w2: position in the ramp / the new blend before the clamp */
  float speed;                                /* w3: car speed (wheel loop) */
  float angle_temp;                           /* w4: new wheel angle before the wrap */
  RwV3d vector;                               /* w5..7: trail spread point / car position */
  RwV3d base;                                 /* w8..10: trail light position */
  union {
    struct { float car[3]; float target[3]; } pair;                    /* w11..16 */
    struct { float unused[3]; RwV3d axis; } wheel;                     /* w14..16: x axis of the wheel spin */
    struct { float unused[6]; RwV3d point; RwV3d player; } trail;      /* w17..19: trail point / up axis, w20..22: player position */
  } u;
} HighwayFrame;

/*008bf0a0*/ extern HighwayCar g_course_highway_cars[HIGHWAY_CARS];
/*006a26a4*/ extern unsigned char g_course_highway_active_bits[10];   /* one bit per car: the car is on the road */
/*008c3fa0*/ extern float g_course_8c3fa0[];                          /* speed per (group + side * 4) */
/*008c4720*/ extern RwV3d g_course_highway_8c4720[];                  /* wheel offsets per (slot + lane * 4) */
/*006a267c*/ extern HighwayFrameRef *g_course_highway_6a267c;
/*006a26b4*/ extern int g_course_highway_6a26b4;
/*006a2684*/ extern RwV3d g_course_highway_6a2684;
/*0060d938*/ extern float g_course_highway_60d938;
/*0060d93c*/ extern float g_course_highway_60d93c;
/*0060d940*/ extern float g_course_highway_60d940;
/*0060d944*/ extern float g_course_highway_60d944;                    /* upper end of the value ramp */
/*0060d948*/ extern float g_course_highway_60d948;                    /* lower end of the value ramp */
/*0060d94c*/ extern float g_course_highway_60d94c;                    /* upper end of the distance ramp */
/*0060d950*/ extern float g_course_highway_60d950;                    /* lower end of the distance ramp */
/*0060d954*/ extern float g_course_highway_60d954;                    /* blend gain of the rise */
/*0060d958*/ extern float g_course_highway_60d958;                    /* blend gain of the fall */
/*0060d95c*/ extern float g_course_highway_60d95c;                    /* blend decay */
/*0062ac74*/ extern RwV3d g_car_62ac74;
/*0062ac68*/ extern RwV3d g_car_62ac68;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern int g_tick_number;

/* rounding point: the original stores the value to a stack float and reloads it (cl would keep the extended value) */
static float float_barrier(float v) { return v; }

#define FRAME_POSITION(ref) (&((float *)(ref)->frame)[0x10])        /* modelling matrix position: frame + 0x40 */

/* course_highway_cb1: per frame update of the highway traffic: activates / releases the cars by the active bit mask, advances the
   ramps (rise / fall) that fade a car in and out by its z distance to the player, emits the tail light trails, moves the cars along
   their lane (wrapping at the lane ends), spins their wheels and places the sky object. */
static void SUB_00459f70_r(struct HighwayCar * p0);
extern void SUB_00459f70(void);
static void SUB_00459fc0_r(struct HighwayCar * p0);
extern void SUB_00459fc0(void);
static void * SUB_00459dd0_r(float * p0, float * p1, int p2, int p3);
extern void SUB_00459dd0(void);
static void SUB_00459f00_r(struct HighwayCar * p0);
extern void SUB_00459f00(void);
static void SUB_00459500_r(struct HighwayCar * p0);
extern void SUB_00459500(void);
void course_highway_cb1(char *world)
{
  HighwayFrame frame;
  HighwayCar *car;
  int i, k, j;

  course_object_distance_table_build();
  for (i = 0; i < HIGHWAY_CARS; i++) {
    car = &g_course_highway_cars[i];
    if ((g_course_highway_active_bits[i >> 3] & (1 << (i & 7))) == 0) {
      if (car->physics != 0) {
        SUB_00459fc0_r(car);
      }
    } else if (car->physics == 0) {
      SUB_00459f70_r(car);
    }
  }
  FUN_004c6600(g_course_highway_active_bits, HIGHWAY_CARS);

  for (car = &g_course_highway_cars[0]; car < &g_course_highway_cars[HIGHWAY_CARS]; car++) {
    frame.distance = 0.0f;
    frame.value = 0.0f;
    if (car->moving == 0) {
      continue;
    }
    car->fall_prev = car->fall;
    car->rise_prev = car->rise;
    car->rise = BLEND_ONE;
    car->fall = BLEND_ZERO;
    {
      float *car_pos = FRAME_POSITION(car->body);
      float *target_pos;
      frame.u.pair.car[0] = car_pos[0];
      frame.u.pair.car[1] = car_pos[1];
      frame.u.pair.car[2] = car_pos[2];
      target_pos = FRAME_POSITION(car->target->frame_ref);
      frame.u.pair.target[0] = target_pos[0];
      frame.u.pair.target[1] = target_pos[1];
      frame.u.pair.target[2] = target_pos[2];
    }
    SUB_00459dd0_r(frame.u.pair.car, &frame.distance, car->side, car->group);
    if (car->side == HIGHWAY_SIDE_FORWARD && frame.u.pair.target[2] > frame.u.pair.car[2]) {
      frame.value = frame.u.pair.target[2] - frame.u.pair.car[2];
    } else if (car->side == HIGHWAY_SIDE_BACKWARD && frame.u.pair.target[2] < frame.u.pair.car[2]) {
      frame.value = frame.u.pair.car[2] - frame.u.pair.target[2];
    }
    car->value_prev = car->value;
    car->value = frame.value;
    car->value_delta = frame.value - car->value_prev;

    /* the car is faded by the ramp of its distance or of its value, whichever is the nearer one */
    if (!(frame.distance > 0.0f)) {
      if (!(frame.value > 0.0f)) {
        goto blend;
      }
      if (frame.distance == 0.0f || frame.value < frame.distance) {
        goto value_ramp;
      }
      goto distance_ramp;
    }
    if (frame.value == 0.0f || frame.distance < frame.value) {
      goto distance_ramp;
    }
value_ramp:
    if (frame.value <= g_course_highway_60d948) {
      car->rise = BLEND_ZERO;
      car->fall = BLEND_ONE;
    } else if (frame.value >= g_course_highway_60d944) {
      car->rise = BLEND_ONE;
      car->fall = BLEND_ZERO;
    } else {
      frame.ramp = (frame.value - g_course_highway_60d948) / (g_course_highway_60d944 - g_course_highway_60d948);
      if (frame.ramp == BLEND_ONE) {
        car->rise = BLEND_ONE;
        car->fall = BLEND_ZERO;
      } else if (frame.ramp == BLEND_ZERO) {
        car->rise = BLEND_ZERO;
        car->fall = BLEND_ONE;
      } else {
        float one_minus = BLEND_ONE - frame.ramp;
        car->fall = one_minus * one_minus * one_minus;
        car->rise = car->value_delta * RAMP_SIDE_SCALE + _rwSqrt(frame.ramp);
      }
    }
    goto blend;
distance_ramp:
    if (frame.distance <= g_course_highway_60d950) {
      car->rise = BLEND_ZERO;
      car->fall = BLEND_ONE;
    } else if (frame.distance >= g_course_highway_60d94c) {
      car->rise = BLEND_ONE;
      car->fall = BLEND_ZERO;
    } else {
      frame.ramp = (frame.distance - g_course_highway_60d950) / (g_course_highway_60d94c - g_course_highway_60d950);
      if (frame.ramp == BLEND_ONE) {
        car->rise = BLEND_ONE;
        car->fall = BLEND_ZERO;
      } else if (frame.ramp == BLEND_ZERO) {
        car->rise = BLEND_ZERO;
        car->fall = BLEND_ONE;
      } else {
        car->rise = frame.ramp * frame.ramp;
        car->fall = _rwSqrt(BLEND_ONE - frame.ramp);
      }
    }
blend:
    car->blend_prev = car->blend;
    car->fall_delta = car->fall - car->fall_prev;
    car->rise_delta = car->rise - car->rise_prev;
    frame.ramp = float_barrier(car->blend - g_course_highway_60d95c - g_course_highway_60d958 * car->fall + g_course_highway_60d954 * car->rise);
    if (frame.ramp >= BLEND_ONE) {
      car->blend = BLEND_ONE;
    } else if (frame.ramp < BLEND_ZERO) {
      car->blend = BLEND_ZERO;
    } else {
      car->blend = frame.ramp;
    }
    car->blend_delta = car->blend - car->blend_prev;
    if (car->visible == 0 || !(car->blend_delta < TRAIL_BLEND_DROP) || race_get_phase() != RACE_PHASE_RACING_VALUE) {
      continue;
    }
    {
      float *car_pos = FRAME_POSITION(car->body);
      frame.u.trail.player.x = car_pos[0];
      frame.u.trail.player.y = car_pos[1];
      frame.u.trail.player.z = car_pos[2];
    }
    for (k = 0; k < car->trail_count; k++) {
      HighwayTrail *trail = &car->trail[k];
      frame.base.x = trail->offset[0] + frame.u.trail.player.x;
      frame.base.y = trail->offset[1] + frame.u.trail.player.y;
      frame.base.z = frame.u.trail.player.z + trail->offset[2];
      frame.vector = frame.base;
      frame.vector.x = (float)(FUN_004c45e0(EXHAUST_SPREAD_X_LOW, EXHAUST_SPREAD_X_HIGH) + frame.vector.x);
      frame.vector.y = (float)(frame.vector.y - FUN_004c45e0(EXHAUST_SPREAD_Y_LOW, EXHAUST_SPREAD_Y_HIGH));
      frame.vector.z = (float)(FUN_004c45e0(EXHAUST_SPREAD_Z_LOW, EXHAUST_SPREAD_Z_HIGH) + frame.vector.z);
      course_highway_cb1_sub_453c20(&frame.vector);
      frame.vector = frame.base;
      frame.vector.x = (float)(frame.vector.x - FUN_004c45e0(EXHAUST_SPREAD_X_LOW, EXHAUST_SPREAD_X_HIGH));
      frame.vector.y = (float)(frame.vector.y - FUN_004c45e0(EXHAUST_SPREAD_Y_LOW, EXHAUST_SPREAD_Y_HIGH));
      frame.vector.z = (float)(FUN_004c45e0(EXHAUST_SPREAD_Z_LOW, EXHAUST_SPREAD_Z_HIGH) + frame.vector.z);
      course_highway_cb1_sub_453c20(&frame.vector);
      for (j = 0; j < 2; j++) {
        int *effect = &trail->effect[j];
        if (*effect == 0) {
          *effect = effect_slot_alloc();
          effect_pool_set_field_14(*effect, 1);
          effect_pool_set_field_1c(*effect, TRAIL_EFFECT_LIFETIME);
        }
        if (*effect != 0 && (g_tick_number & TRAIL_TICK_MASK) != 0) {
          frame.u.trail.point.x = frame.base.x;
          frame.u.trail.point.y = frame.base.y;
          frame.u.trail.point.z = frame.base.z;
          if (j == 0) {
            frame.u.trail.point.x = frame.base.x + TRAIL_SIDE_OFFSET;
          } else {
            frame.u.trail.point.x = frame.base.x - TRAIL_SIDE_OFFSET;
          }
          frame.u.trail.point.y = TRAIL_HEIGHT;
          *effect = effect_trail_add_point(*effect, &frame.u.trail.point, 0, TRAIL_POINT_WIDTH, TRAIL_POINT_FADE);
        }
      }
    }
  }

  for (car = &g_course_highway_cars[0]; car < &g_course_highway_cars[HIGHWAY_CARS]; car++) {
    float speed;
    float *pos;
    pos = FRAME_POSITION(car->body);
    frame.vector.x = 0.0f;
    frame.vector.y = 0.0f;
    frame.vector.z = 0.0f;
    frame.vector.x = pos[0];
    frame.vector.y = pos[1];
    speed = g_course_8c3fa0[car->group + car->side * HIGHWAY_LANES_PER_SIDE] * car->blend;
    frame.speed = speed;
    frame.vector.z = pos[2];
    if (car->moving != 0) {
      if (car->side == HIGHWAY_SIDE_FORWARD) {
        frame.vector.z = frame.speed * g_tick_seconds + frame.vector.z;
      } else {
        frame.vector.z = frame.vector.z - frame.speed * g_tick_seconds;
      }
      SUB_00459f00_r(car);
    } else {
      float *target_pos = FRAME_POSITION(car->target->frame_ref);
      frame.u.trail.player.x = target_pos[0];
      frame.u.trail.player.y = target_pos[1];
      frame.u.trail.player.z = target_pos[2];
      if (car->side == HIGHWAY_SIDE_FORWARD) {
        if (frame.u.trail.player.z > START_FORWARD_Z) {
          car->moving = 1;
        }
      } else if (frame.u.trail.player.z < START_BACKWARD_Z) {
        car->moving = 1;
      }
    }
    if (car->side == HIGHWAY_SIDE_FORWARD && frame.vector.z > LANE_FAR_Z) {
      frame.vector.z = LANE_NEAR_Z;
      car->moving = 0;
    } else if (car->side == HIGHWAY_SIDE_BACKWARD && frame.vector.z < LANE_NEAR_Z) {
      frame.vector.z = LANE_FAR_Z;
      car->moving = 0;
    }
    RwFrameTranslate(car->body->frame, &frame.vector, rwCOMBINEREPLACE);
    car->position = frame.vector;
    SUB_00459500_r(car);
    if (car->visible != 0) {
      if (car->side == HIGHWAY_SIDE_BACKWARD) {
        frame.u.trail.point.x = 0.0f;
        frame.u.trail.point.y = 1.0f;
        frame.u.trail.point.z = 0.0f;
        RwFrameRotate(car->body->frame, &frame.u.trail.point, HALF_TURN, rwCOMBINEPRECONCAT);
      }
      if (car->physics != 0) {
        rwp_object_set_matrix(car->physics->handle, (RwMatrix *)&((float *)car->body->frame)[4]);
      }
      frame.angle_temp = frame.speed * WHEEL_SPIN_RATE + car->angle;
      car->angle = frame.angle_temp;
      if (FULL_TURN <= frame.angle_temp) {
        car->angle = frame.angle_temp - FULL_TURN;
      }
      if (SUB_005685e0(car->body) == 0 && car->trail_count > 0) {
        for (k = 0; k < car->trail_count; k++) {
          struct RwFrame *wheel_frame = car->trail[k].lod_ref->frame;
          frame.u.wheel.axis.x = 1.0f;
          frame.u.wheel.axis.y = 0.0f;
          frame.u.wheel.axis.z = 0.0f;
          RwFrameTranslate(wheel_frame, &g_course_highway_8c4720[k + car->lane * HIGHWAY_LANES_PER_SIDE], rwCOMBINEREPLACE);
          RwFrameRotate(wheel_frame, &frame.u.wheel.axis, -car->angle, rwCOMBINEPRECONCAT);
        }
      }
    }
  }

  if (g_course_highway_6a26b4 != 0) {
    struct RwFrame *sky = ((HighwayFrameRef *)*(HighwayFrameRef **)(world + 0xc1bc))->frame;
    RwFrameTranslate(sky, &g_course_highway_6a2684, rwCOMBINEREPLACE);
    RwFrameRotate(sky, &g_car_62ac74, SKY_TURN_Y, rwCOMBINEPRECONCAT);
    RwFrameRotate(sky, &g_car_62ac68, SKY_TURN_X, rwCOMBINEPRECONCAT);
    g_course_highway_6a2684.x = g_course_highway_60d938 * g_tick_seconds + g_course_highway_6a2684.x;
    g_course_highway_6a2684.y = g_course_highway_60d93c * g_tick_seconds + g_course_highway_6a2684.y;
    g_course_highway_6a2684.z = g_course_highway_60d940 * g_tick_seconds + g_course_highway_6a2684.z;
  }
  if (g_course_highway_6a267c != 0) {
    course_highway_cb1_sub_454150(&((float *)g_course_highway_6a267c->frame)[4]);
  }
  course_highway_cb1_sub_453bf0();
  if (current_course_id() == COURSE_ID_HIGHWAY_NIGHT && g_course_highway_6a267c != 0) {
    course_highway_cb1_sub_453d00(&((float *)g_course_highway_6a267c->frame)[4]);
  }
}

/* SUB_00459f70_r: C -> original SUB_00459f70 (registers edx) */
__declspec(naked) void SUB_00459f70_r(struct HighwayCar * p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call SUB_00459f70
        ret
    }
}

/* SUB_00459fc0_r: C -> original SUB_00459fc0 (registers eax) */
__declspec(naked) void SUB_00459fc0_r(struct HighwayCar * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call SUB_00459fc0
        ret
    }
}

/* SUB_00459dd0_r: C -> original SUB_00459dd0 (registers s0,s1,ebx,eax) */
__declspec(naked) void * SUB_00459dd0_r(float * p0, float * p1, int p2, int p3)
{
    __asm {
        push ebx
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov ebx, dword ptr [esp + 24]
        mov eax, dword ptr [esp + 28]
        call SUB_00459dd0
        add esp, 8
        pop ebx
        ret
    }
}

/* SUB_00459f00_r: C -> original SUB_00459f00 (registers eax) */
__declspec(naked) void SUB_00459f00_r(struct HighwayCar * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call SUB_00459f00
        ret
    }
}

/* SUB_00459500_r: C -> original SUB_00459500 (registers edx) */
__declspec(naked) void SUB_00459500_r(struct HighwayCar * p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call SUB_00459500
        ret
    }
}
