#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005ddc28*/ extern float k_f_0;
/*0062a940*/ extern float g_race_update_62a940;
/*0062a944*/ extern float g_race_update_62a944;
/*0062a948*/ extern float g_race_update_62a948;
/*0062a94c*/ extern float g_race_update_62a94c;
/*0075969c*/ extern undefined4 g_fx_ptank_atomic_b;
/*007596a0*/ extern undefined4 g_particles_7596a0;
/*007da6bc*/ extern char *g_ptank_plugin_offset;
/*008869cc*/ extern undefined4 g_particles_8869cc[0x1000];
/*008885a0*/ extern undefined4 g_particles_8885a0;
/* fx_ptank_b_lock_and_fill */

/* "locks ptank atomic b and writes position/size/colour/uv for each active fx entry (two vertices
   per entry)" */

void __cdecl fx_ptank_b_lock_and_fill(RwMatrix *camera_mtx,undefined4 unused_2)

{
  int entry_mtx;
  float size_scale;
  int *entry_ptr;
  RwReal dist;
  struct {
    float fade;
    undefined1 *colour_ptr;
    int colour_stride;
    undefined4 *uv_ptr;
    char *uv_stride;
    float *pos_ptr;
    char *pos_stride;
    float *size_ptr;
    char *size_stride;
    RwV3d entry_pos;
    float ahead_x;
    float ahead_y;
    float ahead_z;
    RwV3d dir;
  } frame;
  
  frame.entry_pos.x = 0.0;
  frame.entry_pos.y = 0.0;
  frame.entry_pos.z = 0.0;
  if (g_particles_7596a0 != 0) {
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.pos_ptr,1,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.size_ptr,4,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.colour_ptr,2,0x40000000);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_b,&frame.uv_ptr,0x80,0x40000000);
    entry_ptr = g_particles_8869cc;
    frame.ahead_x = camera_mtx->at.x * k_f_3 + camera_mtx->pos.x;
    frame.ahead_y = camera_mtx->at.y * k_f_3 + camera_mtx->pos.y;
    frame.ahead_z = camera_mtx->at.z * k_f_3 + camera_mtx->pos.z;
    g_particles_8885a0 = 0;
    do {
      if (entry_ptr[-6] != 0) {
        if ((RwMatrix *)*entry_ptr == (RwMatrix *)0x0) {
          frame.entry_pos.x = *(float *)&entry_ptr[-10];
          frame.entry_pos.y = *(float *)&entry_ptr[-9];
          frame.entry_pos.z = *(float *)&entry_ptr[-8];
          frame.fade = float_1_0;
        }
        else {
          RwV3dTransformPoints(&frame.entry_pos,(RwV3d *)(entry_ptr + -10),1,(RwMatrix *)*entry_ptr);
          entry_mtx = *entry_ptr;
          frame.fade = *(float *)(entry_mtx + 0x28) * camera_mtx->at.z +
                     *(float *)(entry_mtx + 0x20) * camera_mtx->at.x +
                     *(float *)(entry_mtx + 0x24) * camera_mtx->at.y;
        }
        frame.fade = frame.fade * *(float *)&entry_ptr[-5];
        if (frame.fade < k_f_0) {
          frame.fade = 0.0;
        }
        frame.entry_pos.y = -frame.entry_pos.y;
        if (frame.pos_ptr != (float *)0x0) {
          frame.dir.x = frame.entry_pos.x - frame.ahead_x;
          frame.dir.y = frame.entry_pos.y - frame.ahead_y;
          frame.dir.z = frame.entry_pos.z - frame.ahead_z;
          dist = RwV3dNormalize(&frame.dir,&frame.dir);
          size_scale = frame.fade + k_f_0_5;
          *frame.pos_ptr = dist * frame.dir.x + frame.ahead_x;
          frame.pos_ptr[1] = (dist * frame.dir.y + frame.ahead_y) - frame.fade * k_f_0;
          frame.pos_ptr[2] = dist * frame.dir.z + frame.ahead_z;
          frame.pos_ptr = (float *)((int)frame.pos_ptr + frame.pos_stride);
          *frame.size_ptr = size_scale * *(float *)entry_ptr[-3] * frame.fade;
          frame.size_ptr[1] = size_scale * *(float *)entry_ptr[-3] * frame.fade * k_f_4;
          frame.size_ptr = (float *)((int)frame.size_ptr + frame.size_stride);
          *frame.colour_ptr = *(undefined1 *)entry_ptr[-1];
          frame.colour_ptr[1] = *(undefined1 *)(entry_ptr[-1] + 1);
          frame.colour_ptr[2] = *(undefined1 *)(entry_ptr[-1] + 2);
          frame.colour_ptr[3] = 0x80;
          frame.colour_ptr = frame.colour_ptr + frame.colour_stride;
          *frame.uv_ptr = *(int *)&g_race_update_62a940;
          *(float *)&frame.uv_ptr[1] = g_race_update_62a944;
          *(float *)&frame.uv_ptr[2] = g_race_update_62a948;
          *(float *)&frame.uv_ptr[3] = g_race_update_62a94c;
          frame.uv_ptr = (undefined4 *)((int)frame.uv_ptr + frame.uv_stride);
          g_particles_8885a0 = g_particles_8885a0 + 1;
          *frame.pos_ptr = dist * frame.dir.x + frame.ahead_x;
          frame.pos_ptr[1] = (dist * frame.dir.y + frame.ahead_y) - frame.fade * k_f_0_5;
          frame.pos_ptr[2] = dist * frame.dir.z + frame.ahead_z;
          frame.pos_ptr = (float *)((int)frame.pos_ptr + frame.pos_stride);
          *frame.size_ptr = size_scale * *(float *)entry_ptr[-3] * frame.fade;
          frame.size_ptr[1] = size_scale * *(float *)entry_ptr[-3] * frame.fade * k_f_4;
          frame.size_ptr = (float *)((int)frame.size_ptr + frame.size_stride);
          *frame.colour_ptr = *(undefined1 *)entry_ptr[-1];
          frame.colour_ptr[1] = *(undefined1 *)(entry_ptr[-1] + 1);
          frame.colour_ptr[2] = *(undefined1 *)(entry_ptr[-1] + 2);
          frame.colour_ptr[3] = 0x78;
          frame.colour_ptr = frame.colour_ptr + frame.colour_stride;
          *frame.uv_ptr = *(int *)&g_race_update_62a940;
          *(float *)&frame.uv_ptr[1] = g_race_update_62a944;
          *(float *)&frame.uv_ptr[2] = g_race_update_62a948;
          *(float *)&frame.uv_ptr[3] = g_race_update_62a94c;
          frame.uv_ptr = (undefined4 *)((int)frame.uv_ptr + frame.uv_stride);
          g_particles_8885a0 = g_particles_8885a0 + 1;
        }
      }
      entry_ptr = entry_ptr + 0xe;
    } while ((char *)entry_ptr < (char *)&g_particles_8885a0 + 0x2c);
    RpPTankAtomicUnlock_00545f20(g_fx_ptank_atomic_b);
    *(uint *)(*(char **)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 0x40) =
         *(uint *)(*(char **)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 0x40) | 0x800000;
    *(int *)(*(char **)(g_ptank_plugin_offset + g_fx_ptank_atomic_b) + 4) = g_particles_8885a0;
  }
  return;
}


