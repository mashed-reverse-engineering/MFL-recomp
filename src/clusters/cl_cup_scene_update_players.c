// cluster-entries: cup_scene_update_players
/* Installed cluster cup_scene_update_players (tools/cluster_install.py from output/cluster_pilot_pg2/cup_scene_update_players.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count(void);
/*004146f0*/ extern int race_event_has_panel(void);
/*0042b200*/ extern int *course_get_68b8b4_ptr(void);
/*0042b210*/ extern float * __cdecl course_get_record_point(int record, int point);
/*0047b520*/ extern void __cdecl car_current_matrix(RwMatrix **out, int car);
/*004757a0*/ extern RwMatrix * __cdecl get_viewport_camera_matrix(int viewport);
/*004c4050*/ extern int * __cdecl FUN_004c4050(void *object);
/*004c42f0*/ extern void __cdecl rwframe_look_at(void *frame, float *a, float *b);
/*004d1500*/ extern void * __cdecl RwFrameScale(void *frame, float *scale, int combine);
/*004d15a0*/ extern void * __cdecl RwFrameTransform(void *frame, RwMatrix *matrix, int combine);
/*004d1460*/ extern void * __cdecl RwFrameTranslate(void *frame, float *translation, int combine);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *matrix, float *axis, float degrees, int combine);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *matrix, float *scale, int combine);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *matrix, float *translation, int combine);
/*0062ac68*/ extern float g_car_62ac68[3];
/*0062ac74*/ extern float g_car_62ac74[3];
/*007f0204*/ extern int g_player_slot0_team[];       /* team of the player slots, 4 ints per slot */
/*005dd340*/ extern float k_f_0_5;

/* rounding point: the original spills this sum to a float before the next operation */
static float float_barrier(float value)
{
  return value;
}

#define COMBINE_REPLACE 0
#define COMBINE_PRECONCAT 1
#define COMBINE_POSTCONCAT 2
#define MATRIX_FLAGS_IDENTITY_BITS 0x20003    /* dirty / orthonormal flags set when the modelling matrix is rewritten */
#define CUP_PLAYER_SLOT_STRIDE 4               /* ints per player slot */
#define CUP_TEAM_COLOUR_COUNT 6
#define CUP_PANEL_COLOUR 0x800000ff            /* RGBA bytes ff 00 00 80 */

/* a scene object with an RwFrame (+4) */
typedef struct CupObject {
  int unknown_00;
  struct RwFrame *frame;
} CupObject;

typedef struct FrameView {
  unsigned char unknown_00[0x10];
  RwMatrix modelling;
} FrameView;

/* the cup scene (the trophy room): root object, its placement, and the objects of the players / panels */
typedef struct CupScene {
  CupObject *root;               /* 0x00 */
  float translation[3];          /* 0x04 */
  unsigned char unknown_10[0x24];
  float scale;                   /* 0x34 */
  unsigned char unknown_38[8];
  CupObject *player_object[4];   /* 0x40 */
  CupObject *panel_object[1];    /* 0x50 */
} CupScene;
/*0064d880*/ extern CupScene *g_cup_64d880;

/* cup_scene_update_players: places the cup scene (the root object's frame is rebuilt from the scene translation / scale and the viewport
   camera), moves each player's object to the player's car (coloured by team) and the panel objects to the middle of the course's record
   points */
void cup_scene_update_players(void)
{
  /* one frame object as the original lays it out: the temporaries share slots in turn */
  struct {
    union {
      int player_index;
      unsigned int panel_colour;
    } s0;
    int *team_slot;                      /* pointer into g_player_slot0_team */
    int count;                           /* player count / panel count */
    union {
      RwMatrix *car_matrix;
      int *course_objects;
    } s1;
    float scale[3];                      /* 1/scene scale of each object */
    float world_scale[3];                /* scene scale for the root object */
    union {
      unsigned int team_colour[CUP_TEAM_COLOUR_COUNT];
      float middle[3];
    } t;
    float translate[3];                  /* offset of the root object */
    float first_point[3];
    float second_point[3];
  } frame;
  FrameView *root_frame;
  RwMatrix *matrix;
  RwMatrix *camera;
  struct RwFrame *object_frame;
  int *record;
  int id;
  int i;
  float *point;

  frame.translate[0] = 1.7f;
  frame.translate[1] = -1.0f;
  frame.translate[2] = 4.0f;
  frame.s0.player_index = 0;
  frame.world_scale[0] = 2.0f;
  frame.world_scale[1] = 2.0f;
  frame.world_scale[2] = 2.0f;
  frame.count = get_player_count();
  course_get_68b8b4_ptr();
  if (g_cup_64d880->root != 0) {
    root_frame = (FrameView *)g_cup_64d880->root->frame;
    frame.world_scale[0] = g_cup_64d880->scale;
    frame.world_scale[1] = g_cup_64d880->scale;
    frame.world_scale[2] = g_cup_64d880->scale;
    matrix = &root_frame->modelling;
    camera = get_viewport_camera_matrix(0);
    matrix->at.z = 1.0f;
    matrix->up.y = 1.0f;
    matrix->right.x = 1.0f;
    matrix->up.x = 0.0f;
    matrix->right.z = 0.0f;
    matrix->right.y = 0.0f;
    matrix->at.y = 0.0f;
    matrix->at.x = 0.0f;
    matrix->up.z = 0.0f;
    matrix->pos.z = 0.0f;
    matrix->pos.y = 0.0f;
    matrix->pos.x = 0.0f;
    matrix->flags |= MATRIX_FLAGS_IDENTITY_BITS;
    RwMatrixTranslate(matrix, g_cup_64d880->translation, COMBINE_REPLACE);
    RwMatrixRotate(matrix, g_car_62ac68, -90.0f, COMBINE_POSTCONCAT);
    RwMatrixScale(matrix, frame.world_scale, COMBINE_POSTCONCAT);
    RwMatrixTranslate(matrix, frame.translate, COMBINE_POSTCONCAT);
    RwFrameTransform((struct RwFrame *)root_frame, camera, COMBINE_POSTCONCAT);
    if (frame.count > 0) {
      frame.t.team_colour[0] = 0xc80000ff;
      frame.t.team_colour[1] = 0xc8ff0000;
      frame.t.team_colour[2] = 0xc800ff00;
      frame.t.team_colour[3] = 0xc800ffff;
      frame.t.team_colour[4] = 0xc8ff00ff;
      frame.t.team_colour[5] = 0xc8000000;
      frame.team_slot = g_player_slot0_team;
      for (; frame.s0.player_index < frame.count; frame.s0.player_index++) {
        frame.scale[0] = 0.04f;
        frame.scale[1] = 0.04f;
        frame.scale[2] = 0.04f;
        car_current_matrix(&frame.s1.car_matrix, frame.s0.player_index);
        object_frame = g_cup_64d880->player_object[frame.s0.player_index]->frame;
        RwFrameTransform(object_frame, frame.s1.car_matrix, COMBINE_REPLACE);
        RwFrameTransform(object_frame, matrix, COMBINE_POSTCONCAT);
        frame.scale[0] = (1.0f / g_cup_64d880->scale) * frame.scale[0];
        frame.scale[1] = (1.0f / g_cup_64d880->scale) * frame.scale[1];
        frame.scale[2] = (1.0f / g_cup_64d880->scale) * frame.scale[2];
        RwFrameScale(object_frame, frame.scale, COMBINE_PRECONCAT);
        record = FUN_004c4050(g_cup_64d880->player_object[frame.s0.player_index]);
        record[1] = frame.t.team_colour[*frame.team_slot];
        frame.team_slot += CUP_PLAYER_SLOT_STRIDE;
      }
    }
    frame.s0.panel_colour = CUP_PANEL_COLOUR;
    frame.s1.course_objects = course_get_68b8b4_ptr();
    frame.scale[0] = 0.01f;
    frame.scale[1] = 0.04f;
    frame.scale[2] = 0.01f;
    frame.count = race_event_has_panel();
    frame.scale[0] = (1.0f / g_cup_64d880->scale) * frame.scale[0];
    frame.scale[1] = (1.0f / g_cup_64d880->scale) * frame.scale[1];
    frame.scale[2] = (1.0f / g_cup_64d880->scale) * frame.scale[2];
    for (i = 0; i < frame.count; i++) {
      if (i == 0) {
        id = 0;
      } else {
        id = frame.s1.course_objects[i - 1];
      }
      point = course_get_record_point(id, 0);
      frame.first_point[0] = point[0];
      frame.first_point[1] = point[1];
      frame.first_point[2] = point[2];
      point = course_get_record_point(id, 3);
      frame.second_point[0] = point[0];
      frame.second_point[1] = point[1];
      frame.second_point[2] = point[2];
      frame.t.middle[0] = (frame.second_point[0] + frame.first_point[0]) * k_f_0_5;
      frame.t.middle[1] = (frame.second_point[1] + frame.first_point[1]) * k_f_0_5;
      frame.t.middle[2] = float_barrier(frame.second_point[2] + frame.first_point[2]) * k_f_0_5;      /* the z sum is rounded to float before the halving */
      object_frame = g_cup_64d880->panel_object[i]->frame;
      RwFrameTranslate(object_frame, frame.t.middle, COMBINE_REPLACE);
      rwframe_look_at(object_frame, frame.first_point, g_car_62ac74);
      RwFrameTransform(object_frame, matrix, COMBINE_POSTCONCAT);
      RwFrameScale(object_frame, frame.scale, COMBINE_PRECONCAT);
      record = FUN_004c4050(g_cup_64d880->panel_object[i]);
      record[1] = frame.s0.panel_colour;
    }
  }
}

