#include "mfl_tier2.h"
/*0044a4f0*/ extern int camera_get_position();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004821a0*/ extern int __cdecl vec3_is_valid_small(undefined4 p0);
/*00482220*/ extern int __cdecl matrix_is_valid_small(undefined4 p0, undefined4 p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd620*/ extern float float_10_0;
/*006ad1e8*/ extern undefined4 g_sound_6ad1e8;
/*006ad1ec*/ extern float g_sound_frame_6ad1ec;
/*006ad1f0*/ extern float g_sound_frame_6ad1f0;
/*006ad1f4*/ extern float g_sound_frame_6ad1f4;
/*006ad200*/ extern float g_sound_frame_6ad200;
/*006ad204*/ extern float g_sound_frame_6ad204;
/*006ad208*/ extern float g_sound_frame_6ad208;
/*006ae0b0*/ extern float g_sound_6ae0b0;
/*006ae0b4*/ extern float g_sound_6ae0b4;
/*006ae0b8*/ extern float g_sound_6ae0b8;
/* sound_listener_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the sound listener from the viewport camera matrix and position (validated with
   vec3_is_valid_small)" */

void sound_listener_update(void)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  uint *puVar5;
  int iVar6;
  uint *puVar7;
  RwReal RVar8;
  float v9[9];     /* pos[3], diff[3], zero[3]: one frame object (the escaping pos pointer keeps the zero stores alive, as in the original) */
  uint m[16];      /* the viewport matrix copy (RwMatrix layout) */
  
  ((uint *)v9)[6] = 0;
  ((uint *)v9)[7] = 0;
  ((uint *)v9)[8] = 0;
  puVar5 = (uint *)get_viewport_camera_matrix(0x0);
  puVar7 = m;
  for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar7 = puVar7 + 1;
  }
  iVar6 = matrix_is_valid_small((undefined4)m,0);
  if (iVar6 == 0) {
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
  iVar6 = vec3_is_valid_small((undefined4)v9);
  if (iVar6 == 0) {
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
  RVar8 = RwV3dLength((RwV3d *)(v9 + 3));
  fVar4 = g_sound_frame_6ad1f4;
  fVar3 = g_sound_frame_6ad1f0;
  iVar6 = g_sound_6ad1e8;
  if (RVar8 <= float_10_0) {
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
  *(float *)(iVar6 + 0x2c) = fVar3;
  *(float *)(iVar6 + 0x30) = fVar4;
  *(uint *)(iVar6 + 0x70) = *(uint *)(iVar6 + 0x70) | 1;
  pfVar1 = (float *)(iVar6 + 0x4c);
  *(uint *)pfVar1 = m[8];
  *(undefined4 *)(iVar6 + 0x50) = m[9];
  *(undefined4 *)(iVar6 + 0x54) = m[10];
  pfVar2 = (float *)(iVar6 + 0x40);
  *(float *)(iVar6 + 0x58) =
       *(float *)(iVar6 + 0x54) * *(float *)(iVar6 + 0x44) -
       *(float *)(iVar6 + 0x50) * *(float *)(iVar6 + 0x48);
  *(float *)(iVar6 + 0x5c) = *(float *)(iVar6 + 0x48) * *pfVar1 - *(float *)(iVar6 + 0x54) * *pfVar2
  ;
  *(float *)(iVar6 + 0x60) = *pfVar2 * *(float *)(iVar6 + 0x50) - *(float *)(iVar6 + 0x44) * *pfVar1
  ;
  *(uint *)(iVar6 + 0x70) = *(uint *)(iVar6 + 0x70) | 4;
  *(uint *)pfVar2 = m[4];
  *(undefined4 *)(iVar6 + 0x44) = m[5];
  *(undefined4 *)(iVar6 + 0x48) = m[6];
  *(float *)(iVar6 + 0x58) =
       *(float *)(iVar6 + 0x54) * *(float *)(iVar6 + 0x44) -
       *(float *)(iVar6 + 0x50) * *(float *)(iVar6 + 0x48);
  *(float *)(iVar6 + 0x5c) = *(float *)(iVar6 + 0x48) * *pfVar1 - *(float *)(iVar6 + 0x54) * *pfVar2
  ;
  *(float *)(iVar6 + 0x60) = *pfVar2 * *(float *)(iVar6 + 0x50) - *(float *)(iVar6 + 0x44) * *pfVar1
  ;
  *(uint *)(iVar6 + 0x70) = *(uint *)(iVar6 + 0x70) | 8;
  *(uint *)pfVar2 = m[4];
  *(undefined4 *)(iVar6 + 0x44) = m[5];
  *(undefined4 *)(iVar6 + 0x48) = m[6];
  *(uint *)pfVar1 = m[8];
  *(undefined4 *)(iVar6 + 0x50) = m[9];
  *(undefined4 *)(iVar6 + 0x54) = m[10];
  *(uint *)(iVar6 + 0x58) = m[0];
  *(uint *)(iVar6 + 0x5c) = m[1];
  *(uint *)(iVar6 + 0x60) = m[2];
  *(uint *)(iVar6 + 0x70) = *(uint *)(iVar6 + 0x70) | 0xc;
  *(undefined4 *)(iVar6 + 0x34) = ((uint *)v9)[6];
  *(undefined4 *)(iVar6 + 0x38) = ((uint *)v9)[7];
  *(undefined4 *)(iVar6 + 0x3c) = ((uint *)v9)[8];
  *(uint *)(iVar6 + 0x70) = *(uint *)(iVar6 + 0x70) | 2;
  return;
}


