// cluster-entries: fx_snow_ptank_update
/* Installed cluster fx_snow_ptank_update (tools/cluster_install.py from output/cluster_pilot_k/fx_snow_ptank_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005ddb24*/ extern float k_f_150;
/*0075bfb0*/ extern undefined g_particles_75bfb0;
/*0075cda8*/ extern undefined g_particles_75cda8;
/*0075cdb8*/ extern undefined4 g_particles_75cdb8;
/*0075cdbc*/ extern undefined g_particles_75cdbc;
/*0075cdcc*/ extern undefined g_particles_75cdcc;
/*0075cde0*/ extern float g_particles_75cde0;
/*0075cde4*/ extern float g_particles_75cde4;
/*0075cde8*/ extern float g_particles_75cde8;
/*0075cdec*/ extern undefined g_particles_75cdec;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/* one particle of a snow / weather system pool (0x20 bytes): alive flag, age in frames, position, velocity */
typedef struct SnowDrop { int active; int age; float x, y, z; float vx, vy, vz; } SnowDrop;
#define SNOW_DROP_COUNT 0x200
#define SNOW_DROP_LIFETIME 0x80 /* frames */

/* fx_snow_ptank_update */

/* "spawns/updates the particles of system n near the camera, ages them (0x80 steps) and locks the
   ptank" */

static int particles_emit_random_r(int p0, int p1);
extern void particles_emit_random(void);
/* Emits drops from emitters of system `system` that are within 150 units of the camera and inside its frustum, then locks the ptank
   (positions, colours, sizes), advances every active drop (velocity + wind, age, retire after SNOW_DROP_LIFETIME), writes the vertex data
   (alpha ramps up then down with age, size = age*alpha_per_age+alpha_base) and publishes the active count to the ptank. */
void fx_snow_ptank_update(int system, RwCamera *camera, undefined1 *color_rgb, float alpha_base, float alpha_per_age)

{
  uint *ptank_flags;
  int *live_count;
  int *ptank;
  void *cam_frame;
  float scale;
  SnowDrop *drop;
  int age;
  int alpha;
  RwFrustumTestResult sphere_test;
  int pool;
  int emitter_idx;
  float *emitter;
  RwReal emitter_dist;
  struct {
    undefined1 *color_out;
    int color_stride;
    float *pos_out;
    int pos_stride;
    float *size_out;
    int size_stride;
    RwV3d to_cam;
  } frame;
  
  pool = *(int *)((char *)&g_particles_75cdec + system * 4);
  emitter_idx = 0;
  if ((g_particles_75cdb8 != 0) && (0 < *(int *)((char *)&g_particles_75cda8 + system * 4))) {
    emitter = (float *)((char *)&g_particles_75bfb0 + system * 0x380);
    do {
      cam_frame = RwCamera_object_object_parent(camera);
      frame.to_cam.x = emitter[-2] - *(float *)((char *)cam_frame + 0x40);
      frame.to_cam.y = emitter[-1] - *(float *)((char *)cam_frame + 0x44);
      frame.to_cam.z = *emitter - *(float *)((char *)cam_frame + 0x48);
      emitter_dist = RwV3dLength(&frame.to_cam);
      if ((emitter_dist < k_f_150) &&
         (sphere_test = RwCameraFrustumTestSphere(camera,(RwSphere *)(emitter + 8)),
         sphere_test == rwSPHEREINSIDE)) {
        particles_emit_random_r(pool,(int)(emitter + -2));
      }
      emitter_idx = emitter_idx + 1;
      emitter = emitter + 0xe;
    } while (emitter_idx < *(int *)((char *)&g_particles_75cda8 + system * 4));
  }
  g_particles_75cdb8 = g_particles_75cdb8 + 1;
  live_count = (int *)((char *)&g_particles_75cdbc + system * 4);
  ptank = (int *)((char *)&g_particles_75cdcc + system * 4);
  *live_count = 0;
  RpPTankAtomicLock_00545d10
            (*(undefined4 *)((char *)&g_particles_75cdcc + system * 4),&frame.pos_out,1,0x40000000);
  RpPTankAtomicLock_00545d10(*ptank,&frame.color_out,2,0x40000000);
  RpPTankAtomicLock_00545d10(*ptank,&frame.size_out,4,0x40000000);
  if (((frame.pos_out != (float *)0x0) && (frame.color_out != (undefined1 *)0x0)) && (frame.size_out != (float *)0x0))
  {
    drop = (SnowDrop *)pool;
    for (system = SNOW_DROP_COUNT; system != 0; system--, drop++) {
      if (drop->active == 1) {
        *live_count = *live_count + 1;
        age = drop->age;
        drop->x = drop->vx + drop->x + g_particles_75cde0;
        drop->age = age + 1;
        drop->y = drop->vy + drop->y + g_particles_75cde4;
        drop->z = drop->vz + drop->z + g_particles_75cde8;
        if (SNOW_DROP_LIFETIME < age + 1) {
          drop->active = 0;
          drop->age = 0;
          *live_count = *live_count + -1;
        }
        *frame.pos_out = drop->x;
        frame.pos_out[1] = drop->y;
        frame.pos_out[2] = drop->z;
        frame.pos_out = (float *)((int)frame.pos_out + frame.pos_stride);
        *frame.color_out = *color_rgb;
        frame.color_out[1] = color_rgb[1];
        frame.color_out[2] = color_rgb[2];
        age = drop->age;
        if (age < 0x20) {
          alpha = age / 2;
        }
        else {
          alpha = 0x40 - age / 2;
        }
        if (alpha < 0) {
          alpha = 0;
        }
        else if (0xff < alpha) {
          alpha = 0xff;
        }
        frame.color_out[3] = (char)alpha;
        frame.color_out = frame.color_out + frame.color_stride;
        scale = (float)drop->age * alpha_per_age + alpha_base;
        *frame.size_out = scale;
        frame.size_out[1] = scale;
        frame.size_out = (float *)((int)frame.size_out + frame.size_stride);
      }
    }
    RpPTankAtomicUnlock_00545f20(*ptank);
  }
  ptank_flags = (uint *)(*(char **)(*ptank + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(int *)(*(char **)(*ptank + g_ptank_plugin_offset) + 4) = *live_count;
  return;
}

/* particles_emit_random_r: C -> original particles_emit_random (registers eax,esi) */
__declspec(naked) int particles_emit_random_r(int p0, int p1)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov esi, dword ptr [esp + 12]
        call particles_emit_random
        pop esi
        ret
    }
}
