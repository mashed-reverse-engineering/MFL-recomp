// cluster-entries: course_dem_cb0
/* Installed cluster course_dem_cb0 (tools/cluster_install.py from output/cluster_pilot_pg2/course_dem_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00452ea0*/ extern void course_dem_state_reset(void);
/*004222e0*/ extern void __cdecl vehicle_get_matrix_ltm_b(int car, RwMatrix *matrix);
/*004727a0*/ extern void __cdecl sfx_play_group_at(int group, int position);
/*00475730*/ extern void *__cdecl get_viewport_camera(int index);
/*004800e0*/ extern void __cdecl hud_label_add_from_dat_string(int string_id, float *position, unsigned char *colour_top, unsigned char *colour_bottom, float scale_x, float scale_y);
/*004865f0*/ extern void __cdecl fx_emitter_spawn_c(float *position, float *direction, float angle, float size_a, float size_b);
/*00491ab0*/ extern void __cdecl car_enter_rwp_mode(int car);
/*00492b20*/ extern void __cdecl car_apply_rwp_impulse(int car, float *point, float *impulse);
/*004980c0*/ extern void __cdecl fx_trail_particle_spawn_interpolated(void *particle, int a, int b, int c);
/*004a1680*/ extern void __cdecl player_rumble(int player, int motor, int duration, int strength);
/*004c45c0*/ extern int __cdecl FUN_004c45c0(int low, int high);
/*004c45e0*/ extern double __cdecl FUN_004c45e0(float low, float high);
/*004d1460*/ extern void * __cdecl RwFrameTranslate(struct RwFrame *frame, float *translation, int combine);
/*004d1cd0*/ extern int __cdecl RwCameraFrustumTestSphere(void *camera, float *sphere);
/*0060d808*/ extern int g_course_dem_60d808;       /* the "get ready" label has not been shown for this round */
/*0060d80c*/ extern int g_course_dem_60d80c;       /* the round-start sound has not been stopped yet */
/*0060d8b0*/ extern float g_course_60d8b0[];       /* END of the lift table: read backwards, entry index -n */
/*006a07ac*/ extern unsigned int g_course_dem_6a07ac;   /* time since the round started */
/*006a07b0*/ extern int g_course_dem_6a07b0;       /* camera shake frames left */
/*006a07b4*/ extern float g_course_dem_6a07b4;     /* height the course objects are lifted by */
/*006a07b8*/ extern int g_course_dem_6a07b8;       /* round number */
/*006a07bc*/ extern int g_course_dem_6a07bc;       /* countdown */
/*006a07c0*/ extern int g_course_dem_6a07c0;       /* frames of dust left */
/*007ef7ac*/ extern float g_camera_shake;
/*005df4d0*/ extern float k_f_3_5;
/*005df4d4*/ extern float k_f_0_47169814;

#define DEM_STOP_SOUND_TIME 21000      /* ms into the round */
#define DEM_LABEL_TIME 39000
#define DEM_ROUND_TIME 45000
#define DEM_LAST_ROUND 7
#define DEM_SHAKE_FRAMES 60
#define DEM_SHAKE_LIFT_FIRST 15        /* lift while the shake counter is between these */
#define DEM_SHAKE_LIFT_LAST 40
#define DEM_SHAKE_STRONG 40
#define DEM_DUST_FRAMES 10
#define DEM_COUNTDOWN 6
#define DEM_CAR_COUNT 4
#define DEM_LABEL_STRING 0x32d
#define DEM_SOUND_STOP_GROUP 10
#define DEM_SOUND_START_GROUP 11
#define COMBINE_REPLACE 0

/* offsets into the course world object */
#define WORLD_OBJECT_COUNT 0xc000      /* number of objects (position at +0x30, stride 0x40) */
#define WORLD_OBJECTS_POSITION 0x30
#define WORLD_OBJECT_STRIDE 0x40
/* the six course objects whose frames are lifted together (pointers in the world object; the frame is at +4 of each) */
#define WORLD_LIFT_OBJECT_A 0xc118
#define WORLD_LIFT_OBJECT_B 0xc120
#define WORLD_LIFT_OBJECT_C 0xc110
#define WORLD_LIFT_OBJECT_D 0xc114
#define WORLD_LIFT_OBJECT_E 0xc12c
#define WORLD_LIFT_OBJECT_F 0xc130
#define LIFT_OBJECT_FRAME(offset) RwFrameTranslate(*(struct RwFrame **)(*(unsigned char **)(world + (offset)) + 4), frame.lift_vector, COMBINE_REPLACE)

/* a trail dust particle record: the position is followed by the particle template that fx_trail_particle_spawn_interpolated reads
   through the same pointer (0x48 bytes; the words nothing sets are left alone) */
typedef struct DustParticle {
  float position[3];             /* 0x00 */
  unsigned int colour;           /* 0x0c */
  float size_a;                  /* 0x10 */
  float size_b;                  /* 0x14 */
  float random_a;                /* 0x18 */
  float random_b;                /* 0x1c */
  float random_c;                /* 0x20 */
  float zero[3];                 /* 0x24 */
  float fade;                    /* 0x30 */
  float unknown_34;              /* 0x34: not set */
  float one;                     /* 0x38 */
  float unknown_3c;              /* 0x3c: not set */
  int flags;                     /* 0x40 */
  unsigned char unknown_44;      /* 0x44: not set */
  unsigned char active;          /* 0x45 */
  unsigned char unknown_46[2];
} DustParticle;

/* (global, not static: cl would otherwise pass ticks in edx like the original, making course_dem_cb0 byte-identical to the original 12 byte stub, which overlay.py then size-checks against the 1326 byte Ghidra function)
   the demolition course's per-frame update (ticks = time since the last call): moves the round clock, plays the round sounds, starts the
   next round after 45 s (shake, dust, impulses and rumble for every car), shakes / lifts the course and sprays dust */
void course_dem_update(unsigned int ticks, unsigned char *world)
{
  /* ONE frame object, as the original lays it out (the callees read the words behind the pointers they get): the temporaries share
     storage in turn, so they are members of overlays */
  struct {
    union {
      unsigned char colour_bottom[4];   /* hud label colours */
      float random_a;                   /* first random value of a dust emitter */
    } a;
    union {
      unsigned char colour_top[4];
      float random_b;
    } b;
    union {
      float label_position[2];          /* hud label position */
      float direction[3];               /* emitter direction */
      float impulse[3];                 /* impulse applied to a car */
    } v;
    float lift_vector[3];               /* translation of the lifted course objects */
    union {
      float sphere[4];                  /* culling sphere: centre, radius */
      float point[3];                   /* the point of a car impulse */
    } w;
    union {
      RwMatrix car_matrix;
      DustParticle dust;
    } m;
  } frame;
  int i;
  int car;
  unsigned char *position;

  frame.lift_vector[0] = 0.0f;
  frame.lift_vector[1] = 0.0f;
  frame.lift_vector[2] = 0.0f;
  g_course_dem_6a07ac += ticks;
  if (g_course_dem_6a07bc != 0) {
    g_course_dem_6a07bc--;
  }
  if (g_course_dem_6a07ac >= DEM_STOP_SOUND_TIME && g_course_dem_60d80c != 0) {
    sfx_play_group_at(DEM_SOUND_STOP_GROUP, 0);
    g_course_dem_60d80c = 0;
  }
  if (g_course_dem_6a07ac >= DEM_LABEL_TIME && g_course_dem_60d808 != 0) {
    frame.a.colour_bottom[0] = 0x80;
    frame.a.colour_bottom[1] = 0x40;
    frame.a.colour_bottom[2] = 0;
    frame.a.colour_bottom[3] = 0xff;
    frame.b.colour_top[0] = 0xff;
    frame.b.colour_top[1] = 0x80;
    frame.b.colour_top[2] = 0;
    frame.b.colour_top[3] = 0xff;
    frame.v.label_position[0] = 320.0f;
    frame.v.label_position[1] = 240.0f;
    hud_label_add_from_dat_string(DEM_LABEL_STRING, frame.v.label_position, frame.b.colour_top, frame.a.colour_bottom, 10.0f, 1.5f);
    g_course_dem_60d808 = 0;
  }
  if (g_course_dem_6a07ac >= DEM_ROUND_TIME && g_course_dem_6a07b8 < DEM_LAST_ROUND) {
    g_course_dem_6a07b0 = DEM_SHAKE_FRAMES;
    g_course_dem_6a07ac = 0;
    g_course_dem_6a07b8++;
    g_course_dem_60d808 = 1;
    g_course_dem_6a07bc = DEM_COUNTDOWN;
    g_course_dem_6a07c0 = DEM_DUST_FRAMES;
    sfx_play_group_at(DEM_SOUND_START_GROUP, 0);
    g_course_dem_60d80c = 1;
    position = world + WORLD_OBJECTS_POSITION;
    for (i = 0; i < *(int *)(world + WORLD_OBJECT_COUNT); i++, position += WORLD_OBJECT_STRIDE) {
      frame.v.direction[0] = 0.0f;
      frame.v.direction[1] = 1.0f;
      frame.v.direction[2] = 0.0f;
      frame.a.random_a = (float)FUN_004c45e0(0.0f, 0.125f);
      frame.b.random_b = (float)FUN_004c45e0(4.0f, 6.0f);
      frame.w.sphere[0] = ((float *)position)[0];
      frame.w.sphere[1] = ((float *)position)[1];
      frame.w.sphere[2] = ((float *)position)[2];
      frame.w.sphere[3] = frame.b.random_b;
      if (RwCameraFrustumTestSphere(get_viewport_camera(0), frame.w.sphere) != 0) {
        fx_emitter_spawn_c((float *)position, frame.v.direction, 360.0f, frame.a.random_a, frame.b.random_b);
      }
    }
    for (car = 0; car < DEM_CAR_COUNT; car++) {
      frame.w.point[0] = 0.0f;
      frame.w.point[1] = 100.0f;
      frame.w.point[2] = 0.0f;
      car_enter_rwp_mode(car);
      vehicle_get_matrix_ltm_b(car, &frame.m.car_matrix);
      frame.v.impulse[0] = frame.m.car_matrix.pos.x;
      frame.v.impulse[1] = frame.m.car_matrix.pos.y;
      frame.v.impulse[2] = frame.m.car_matrix.pos.z;
      frame.v.impulse[0] = (float)(FUN_004c45e0(-0.1f, 0.1f) + frame.v.impulse[0]);
      frame.v.impulse[2] = (float)(FUN_004c45e0(-0.1f, 0.1f) + frame.v.impulse[2]);
      car_apply_rwp_impulse(car, frame.w.point, frame.v.impulse);
      player_rumble(car, 2, 30, FUN_004c45c0(200, 255));
    }
  }
  if (g_course_dem_6a07b0 != 0 && g_course_dem_6a07b8 <= DEM_LAST_ROUND) {
    g_course_dem_6a07b0--;
    g_camera_shake = (float)FUN_004c45e0(0.05f, 0.1f);
    if (g_course_dem_6a07b0 > DEM_SHAKE_STRONG) {
      g_camera_shake = (float)FUN_004c45e0(0.25f, 0.35f);
    }
    if (g_course_dem_6a07b0 > DEM_SHAKE_LIFT_FIRST && g_course_dem_6a07b0 < DEM_SHAKE_LIFT_LAST) {
      g_course_dem_6a07b4 = k_f_3_5 / (1.0f / ((g_course_60d8b0 - g_course_dem_6a07b0)[0] * k_f_0_47169814)) + g_course_dem_6a07b4;
    }
  } else {
    g_camera_shake = 0.0f;
  }
  frame.lift_vector[1] = g_course_dem_6a07b4;
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_A);
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_B);
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_C);
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_D);
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_E);
  LIFT_OBJECT_FRAME(WORLD_LIFT_OBJECT_F);
  if (g_course_dem_6a07c0 != 0) {
    frame.m.dust.colour = 0xfa323232;
    frame.m.dust.size_a = 2.5f;
    frame.m.dust.size_b = 2.5f;
    frame.m.dust.random_a = (float)FUN_004c45e0(-0.2f, 0.2f);
    frame.m.dust.random_b = (float)FUN_004c45e0(2.0f, 3.0f);
    frame.m.dust.random_c = (float)FUN_004c45e0(-0.2f, 0.2f);
    frame.m.dust.zero[0] = 0.0f;
    frame.m.dust.zero[1] = 0.0f;
    frame.m.dust.zero[2] = 0.0f;
    frame.m.dust.fade = 0.2f;
    frame.m.dust.one = 1.0f;
    frame.m.dust.flags = 0x1120;
    frame.m.dust.active = 1;
    position = world + WORLD_OBJECTS_POSITION;
    for (i = 0; i < *(int *)(world + WORLD_OBJECT_COUNT); i++, position += WORLD_OBJECT_STRIDE) {
      frame.m.dust.position[0] = ((float *)position)[0];
      frame.m.dust.position[1] = ((float *)position)[1];
      frame.m.dust.position[2] = ((float *)position)[2];
      frame.m.dust.position[0] = (float)(FUN_004c45e0(-1.0f, 1.0f) + frame.m.dust.position[0]);
      frame.m.dust.position[1] = (float)(FUN_004c45e0(-1.0f, 1.0f) + frame.m.dust.position[1]);
      frame.m.dust.position[2] = (float)(FUN_004c45e0(-1.0f, 1.0f) + frame.m.dust.position[2]);
      fx_trail_particle_spawn_interpolated(&frame.m.dust, 0, 0, 0);
    }
    g_course_dem_6a07c0--;
  }
}

/* course_dem_cb0: the course set-up callback of the demolition course: resets the state, then runs the first update (ticks 0) */
void course_dem_cb0(unsigned char *world)
{
  course_dem_state_reset();
  course_dem_update(0, world);
}

