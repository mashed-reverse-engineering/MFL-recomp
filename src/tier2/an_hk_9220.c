#include "mfl_tier2.h"
/*0044a4f0*/ extern int camera_get_position();
/*004757a0*/ extern char *get_viewport_camera_matrix();
/*004821a0*/ extern int __cdecl vec3_is_valid_small(void *p0);
/*00482220*/ extern int __cdecl matrix_is_valid_small(void *p0, undefined4 p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd620*/ extern float float_10_0;
/*006ad1e8*/ extern char *g_sound_6ad1e8;
/*006ad1ec*/ extern float g_sound_frame_6ad1ec;
/*006ad1f0*/ extern float g_sound_frame_6ad1f0;
/*006ad1f4*/ extern float g_sound_frame_6ad1f4;
/*006ad200*/ extern float g_sound_frame_6ad200;
/*006ad204*/ extern float g_sound_frame_6ad204;
/*006ad208*/ extern float g_sound_frame_6ad208;
/*006ae0b0*/ extern float g_sound_6ae0b0;
/*006ae0b4*/ extern float g_sound_6ae0b4;
/* layout of *listener in sound_listener_update, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct sound_listener_layout {
    char pad_0x0[44];
    float f_0x2c;
    float f_0x30;
    undefined4 f_0x34;
    undefined4 f_0x38;
    undefined4 f_0x3c;
    float f_0x40;
    union { float as_float; undefined4 as_undefined4; } f_0x44;
    union { float as_float; undefined4 as_undefined4; } f_0x48;
    float f_0x4c;
    union { float as_float; undefined4 as_undefined4; } f_0x50;
    union { float as_float; undefined4 as_undefined4; } f_0x54;
    union { float as_float; uint as_uint; } f_0x58;
    union { float as_float; uint as_uint; } f_0x5c;
    union { float as_float; uint as_uint; } f_0x60;
    char pad_0x64[12];
    uint f_0x70;
} sound_listener_layout;

/*006ae0b8*/ extern float g_sound_6ae0b8;
/* sound_listener_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the sound listener from the viewport camera matrix and position (validated with
   vec3_is_valid_small)" */

/* (listener: the sound system's listener record at g_sound_6ad1e8; fwd/up are its axis vectors, the 0x58..0x60 triple is their cross product) */
void sound_listener_update(void)

{
  float *fwd;
  float *up;
  float pos_y;
  float pos_z;
  uint *src_m;
  int n;
  sound_listener_layout *listener;
  uint *dst_m;
  RwReal move_dist;
  float v9[9];     /* pos[3], diff[3], zero[3]: one frame object (the escaping pos pointer keeps the zero stores alive, as in the original) */
  uint m[16];      /* the viewport matrix copy (RwMatrix layout) */
  
  ((uint *)v9)[6] = 0;
  ((uint *)v9)[7] = 0;
  ((uint *)v9)[8] = 0;
  src_m = (uint *)get_viewport_camera_matrix(0x0);
  dst_m = m;
  for (n = 0x10; n != 0; n = n + -1) {
    *dst_m = *src_m;
    src_m = src_m + 1;
    dst_m = dst_m + 1;
  }
  n = matrix_is_valid_small(m,0);
  if (n == 0) {
    m[3] = m[3] | 0x20003;
    m[10] = 0x3f800000;
    m[5] = 0x3f800000;
    m[0] = 0x3f800000;
    m[1] = 0;
    m[2] = 0;
    m[4] = 0;
    m[6] = 0;
    m[8] = 0;
    m[9] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
  }
  camera_get_position(v9);
  n = vec3_is_valid_small(v9);
  if (n == 0) {
    v9[0] = 0.0;
    v9[1] = 0.0;
    v9[2] = 0.0;
  }
  v9[3] = v9[0] - g_sound_6ae0b0;
  g_sound_frame_6ad1ec = v9[0];
  v9[4] = v9[1] - g_sound_6ae0b4;
  g_sound_frame_6ad1f0 = v9[1];
  g_sound_frame_6ad1f4 = v9[2];
  v9[5] = v9[2] - g_sound_6ae0b8;
  move_dist = RwV3dLength((RwV3d *)(v9 + 3));
  pos_z = g_sound_frame_6ad1f4;
  pos_y = g_sound_frame_6ad1f0;
  listener = (sound_listener_layout *)(g_sound_6ad1e8);
  if (!(move_dist > float_10_0)) {
    g_sound_6ae0b0 = g_sound_frame_6ad1ec;
    g_sound_6ae0b4 = g_sound_frame_6ad1f0;
    g_sound_6ae0b8 = g_sound_frame_6ad1f4;
  }
  else {
    g_sound_6ae0b0 = v9[3] * k_f_0_5 + g_sound_6ae0b0;
    g_sound_6ae0b4 = v9[4] * k_f_0_5 + g_sound_6ae0b4;
    g_sound_6ae0b8 = v9[5] * k_f_0_5 + g_sound_6ae0b8;
  }
  g_sound_frame_6ad200 = g_sound_6ae0b0;
  g_sound_frame_6ad204 = g_sound_6ae0b4;
  g_sound_frame_6ad208 = g_sound_6ae0b8;
  *(float *)(g_sound_6ad1e8 + 0x28) = g_sound_frame_6ad1ec;
  listener->f_0x2c = pos_y;
  listener->f_0x30 = pos_z;
  listener->f_0x70 = listener->f_0x70 | 1;
  fwd = &listener->f_0x4c;
  *(uint *)fwd = m[8];
  listener->f_0x50.as_undefined4 = m[9];
  listener->f_0x54.as_undefined4 = m[10];
  up = &listener->f_0x40;
  listener->f_0x58.as_float =
       listener->f_0x54.as_float * listener->f_0x44.as_float -
       listener->f_0x50.as_float * listener->f_0x48.as_float;
  listener->f_0x5c.as_float = listener->f_0x48.as_float * *fwd - listener->f_0x54.as_float * *up
  ;
  listener->f_0x60.as_float = *up * listener->f_0x50.as_float - listener->f_0x44.as_float * *fwd
  ;
  listener->f_0x70 = listener->f_0x70 | 4;
  *(uint *)up = m[4];
  listener->f_0x44.as_undefined4 = m[5];
  listener->f_0x48.as_undefined4 = m[6];
  listener->f_0x58.as_float =
       listener->f_0x54.as_float * listener->f_0x44.as_float -
       listener->f_0x50.as_float * listener->f_0x48.as_float;
  listener->f_0x5c.as_float = listener->f_0x48.as_float * *fwd - listener->f_0x54.as_float * *up
  ;
  listener->f_0x60.as_float = *up * listener->f_0x50.as_float - listener->f_0x44.as_float * *fwd
  ;
  listener->f_0x70 = listener->f_0x70 | 8;
  *(uint *)up = m[4];
  listener->f_0x44.as_undefined4 = m[5];
  listener->f_0x48.as_undefined4 = m[6];
  *(uint *)fwd = m[8];
  listener->f_0x50.as_undefined4 = m[9];
  listener->f_0x54.as_undefined4 = m[10];
  listener->f_0x58.as_uint = m[0];
  listener->f_0x5c.as_uint = m[1];
  listener->f_0x60.as_uint = m[2];
  listener->f_0x70 = listener->f_0x70 | 0xc;
  listener->f_0x34 = ((uint *)v9)[6];
  listener->f_0x38 = ((uint *)v9)[7];
  listener->f_0x3c = ((uint *)v9)[8];
  listener->f_0x70 = listener->f_0x70 | 2;
  return;
}


