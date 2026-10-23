#include "mfl_tier2.h"
/*00429dd0*/ extern undefined __cdecl frame_rotate_around_own_origin(int p0, RwReal p1);
/*00484c40*/ extern undefined __cdecl spline_tween_b(RwMatrix *p1, int p2, float p3);
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*005dda20*/ extern float k_f_4_2949673e9;
/*00609550*/ extern undefined g_race_update_609550;
/*007ef804*/ extern undefined4 g_tick_number;
/*008c88a4*/ extern undefined4 g_game_8c88a4[0x1000];
/* course_animated_objects_update */

/* "advances path times for the animated course objects, rotates children, spawns particles every
   second" */

int course_animated_objects_update(uint dt_ms)

{
  RwFrame *frame;
  float dt_f;
  float path_step;
  int path_or_tmp;
  int tick_number;
  int *object_ptr;
  int emitter_i;
  bool on_second;
  struct {
    RwV3d emit_pos;
    undefined4 local_7c;
    undefined4 local_78;
    undefined4 local_74;
    undefined4 local_70;
    undefined4 local_6c;
    undefined4 local_68;
    undefined4 local_64;
    undefined4 local_60;
    undefined4 local_5c;
    undefined4 local_58;
    undefined1 pad_54[4];
    undefined4 local_50;
    undefined1 pad_4c[4];
    undefined4 local_48;
    undefined1 pad_44[1];
    undefined1 local_43;
    undefined1 pad_42[2];
    RwMatrix path_mtx;
  } frame_obj;
  volatile float vang;
  float angle;
  
  path_step = (dt_f = (float)dt_ms) * 3.3333333e-4f;
  object_ptr = g_game_8c88a4;
  tick_number = g_tick_number;
  do {
    path_or_tmp = 0;
    if (object_ptr[-0x11] != 0) {
      path_or_tmp = *object_ptr;
      if (path_or_tmp != 0) {
        frame = *(RwFrame **)(object_ptr[-0x11] + 4);
        vang = path_step + *(float *)&object_ptr[-1];
        *(float *)&object_ptr[-1] = vang;
        if (*(float *)(path_or_tmp + 0xc) < vang) {
          *(float *)&object_ptr[-1] = (vang - *(float *)(path_or_tmp + 0xc));
        }

        spline_tween_b(&frame_obj.path_mtx,path_or_tmp,*(float *)&object_ptr[-1]);
        RwFrameTransform(frame,&frame_obj.path_mtx,rwCOMBINEREPLACE);
        angle = dt_f * 0.05f;
        *(float *)&object_ptr[-4] = frame_obj.path_mtx.pos.x;
        *(float *)&object_ptr[-3] = frame_obj.path_mtx.pos.y;
        *(float *)&object_ptr[-2] = frame_obj.path_mtx.pos.z;
        frame_rotate_around_own_origin(object_ptr[-0xf],angle);
        frame_rotate_around_own_origin(object_ptr[-0xe],dt_f * -0.05f);
        tick_number = g_tick_number;
      }
      path_or_tmp = object_ptr[-0x11];
      if ((path_or_tmp != 0) && (object_ptr[2] != -1)) {
        emitter_i = 0;
        do {
          if (emitter_i == 0) {
            path_or_tmp = tick_number / 0x3c;
            on_second = tick_number % 0x3c == 0;
LAB_00429fcb:
            if (on_second) {
              RwV3dTransformPoint(&frame_obj.emit_pos,
                                  (RwV3d *)((char *)&g_race_update_609550 + (emitter_i + object_ptr[2] * 3) * 0xc),
                                  (RwMatrix *)(*(char **)(object_ptr[-0x11] + 4) + 0x10));
              frame_obj.local_43 = 3;
              frame_obj.local_58 = 0x42055556;
              frame_obj.local_7c = 0xffffffff;
              frame_obj.local_48 = 0x920;
              frame_obj.local_78 = 0x3f19999a;
              frame_obj.local_74 = 0x3f19999a;
              frame_obj.local_50 = 0x3f800000;
              frame_obj.local_70 = 0;
              frame_obj.local_6c = 0;
              frame_obj.local_68 = 0;
              frame_obj.local_64 = 0;
              frame_obj.local_60 = 0;
              frame_obj.local_5c = 0;
              path_or_tmp = fx_trail_particle_spawn_interpolated(&frame_obj.emit_pos,0,0,0);
              tick_number = g_tick_number;
            }
          }
          else {
            if (emitter_i == 1) {
              path_or_tmp = tick_number / 100;
              on_second = tick_number % 100 == 10;
              goto LAB_00429fcb;
            }
            path_or_tmp = emitter_i + -2;
            if (path_or_tmp == 0) {
              path_or_tmp = tick_number / 100;
              on_second = tick_number % 100 == 0xf;
              goto LAB_00429fcb;
            }
          }
          emitter_i = emitter_i + 1;
        } while (emitter_i < 3);
      }
    }
    object_ptr = object_ptr + 0x14;
    if ((int)g_game_8c88a4 + 0x27f < (int)object_ptr) {
      return path_or_tmp;
    }
  } while( true );
}


