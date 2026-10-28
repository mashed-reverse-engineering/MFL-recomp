// cluster-entries: rain_update_render
/* Installed cluster rain_update_render (tools/cluster_install.py from output/cluster_pilot_k/rain_update_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd374*/ extern float k_f_0_2;
/*005de490*/ extern float k_f_0_02;
/*005e0b24*/ extern float k_f_75;
/*0076e3e8*/ extern undefined4 g_app_76e3e8;
/*0076e5a0*/ extern undefined4 g_app_76e5a0;
/*0076e5a4*/ extern undefined4 g_rain_update_76e5a4;
/*0076e5a8*/ extern undefined4 g_app_76e5a8;
/*0076e5ac*/ extern undefined4 g_app_76e5ac;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*00885820*/ extern undefined4 g_app_885820;
/* one rain drop of the pool (0x20 bytes): alive flag, age in frames, position, velocity */
typedef struct RainDrop { int active; int age; float x, y, z; float vx, vy, vz; } RainDrop;
#define RAIN_DROP_COUNT 0x200
#define RAIN_DROP_LIFETIME 0x40 /* frames */

/* rain_update_render */

/* "updates the rain particles visible in the camera frustum" */

static int rain_emit_random_r(int p0, int p1);
extern void rain_emit_random(void);
/* Per frame: emit drops from rain emitters (stride 0xe floats at g_app_76e3e8) that are within 75.0 of the camera and whose sphere is inside the frustum; then lock the PTank (flags 1/2/4 = position/colour/size streams, inferred), age and move every active drop and write survivors (lifetime RAIN_DROP_LIFETIME frames) into the PTank, unlock, and set the active count. (inferred) */
void rain_update_render(RwCamera *camera)

{
  uint *ptank_flags;
  float old_x_or_size;
  void *camera_frame;
  RainDrop *drop;
  int age;
  RwFrustumTestResult sphere_visibility;
  int index_or_alpha;
  float *emitter_z;
  int pool_or_remaining;
  RwReal length;
  struct {
    char *color_ptr;
    int color_stride;
    float *pos_ptr;
    int pos_stride;
    float *size_ptr;
    int size_stride;
    RwV3d to_emitter;
  } frame;
  
  pool_or_remaining = g_app_885820;
  index_or_alpha = 0;
  if ((g_rain_update_76e5a4 != 0) && (0 < (int)g_app_76e5a0)) {
    emitter_z = (float *)&g_app_76e3e8;
    do {
      camera_frame = RwCamera_object_object_parent(camera);
      frame.to_emitter.x = emitter_z[-2] - *(float *)((char *)camera_frame + 0x40);
      frame.to_emitter.y = emitter_z[-1] - *(float *)((char *)camera_frame + 0x44);
      frame.to_emitter.z = *emitter_z - *(float *)((char *)camera_frame + 0x48);
      length = RwV3dLength(&frame.to_emitter);
      if ((length < k_f_75) &&
         (sphere_visibility = RwCameraFrustumTestSphere(camera,(RwSphere *)(emitter_z + 8)),
         sphere_visibility == rwSPHEREINSIDE)) {
        rain_emit_random_r(pool_or_remaining,(int)(emitter_z + -2));
      }
      index_or_alpha = index_or_alpha + 1;
      emitter_z = emitter_z + 0xe;
    } while (index_or_alpha < (int)g_app_76e5a0);
  }
  g_rain_update_76e5a4 = g_rain_update_76e5a4 + 1;
  g_app_76e5a8 = 0;
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.pos_ptr,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.color_ptr,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_app_76e5ac,&frame.size_ptr,4,0x40000000);
  if (((frame.pos_ptr != (float *)0x0) && (frame.color_ptr != (char *)0x0)) && (frame.size_ptr != (float *)0x0)) {
    drop = (RainDrop *)pool_or_remaining;
    for (pool_or_remaining = RAIN_DROP_COUNT; pool_or_remaining != 0; pool_or_remaining--, drop++) {
      if (drop->active == 1) {
        age = drop->age;
        old_x_or_size = drop->x;
        drop->x = drop->vx + old_x_or_size;
        drop->age = age + 1;
        drop->y = drop->vy + drop->y;
        drop->z = drop->vz + drop->z;
        if (age + 1 < RAIN_DROP_LIFETIME) {
          *frame.pos_ptr = drop->vx + old_x_or_size;
          frame.pos_ptr[1] = drop->y;
          frame.pos_ptr[2] = drop->z;
          frame.pos_ptr = (float *)((int)frame.pos_ptr + frame.pos_stride);
          *frame.color_ptr = (char)(drop->age / 2) + -0x40;
          frame.color_ptr[1] = (char)(drop->age / 2) + '@';
          frame.color_ptr[2] = '\b';
          index_or_alpha = drop->age * -4 + 0xff;
          if (index_or_alpha < 0) {
            index_or_alpha = 0;
          }
          else if (0xff < index_or_alpha) {
            index_or_alpha = 0xff;
          }
          frame.color_ptr[3] = (char)index_or_alpha;
          frame.color_ptr = frame.color_ptr + frame.color_stride;
          old_x_or_size = (float)drop->age * k_f_0_02 + k_f_0_2;
          *frame.size_ptr = old_x_or_size;
          frame.size_ptr[1] = old_x_or_size;
          frame.size_ptr = (float *)((int)frame.size_ptr + frame.size_stride);
          g_app_76e5a8 = g_app_76e5a8 + 1;
        }
        else {
          drop->active = 0;
          drop->age = 0;
        }
      }
    }
    RpPTankAtomicUnlock_00545f20(g_app_76e5ac);
  }
  pool_or_remaining = g_app_76e5ac;
  ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + g_app_76e5ac) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(int *)(*(char **)(g_ptank_plugin_offset + pool_or_remaining) + 4) = g_app_76e5a8;
  return;
}

/* rain_emit_random_r: C -> original rain_emit_random (registers eax,edi) */
__declspec(naked) int rain_emit_random_r(int p0, int p1)
{
    __asm {
        push edi
        mov eax, dword ptr [esp + 8]
        mov edi, dword ptr [esp + 12]
        call rain_emit_random
        pop edi
        ret
    }
}
