// cluster-entries: course_helper_449c40
/* Installed cluster course_helper_449c40 (tools/cluster_install.py from output/cluster_pilot_md/course_helper_449c40.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00416850*/ extern float * SUB_00416850(void);
/*00416860*/ extern float SUB_00416860(void);
/*00416870*/ extern void __cdecl SUB_00416870(float p0);
/*0044c2a0*/ extern float SUB_0044c2a0(void);
/*0049a960*/ extern void __cdecl RpPTankAtomicLock_00545d10(void *atomic, void *locked, int flags, int mode);
/*0049a970*/ extern void __cdecl RpPTankAtomicUnlock_00545f20(void *atomic);
/*005dd33c*/ extern float k_f_4;
/*005dd378*/ extern float k_f_0_1;
/*005dd418*/ extern float k_f_12;
/*005dd61c*/ extern float k_f_0_001;
/*005df2b4*/ extern float k_f_5_0en4;
/*005df2b8*/ extern float k_f_1_6666666;
/*006a06e0*/ extern void *g_sky_atomic;
/*007da6bc*/ extern int g_ptank_plugin_offset;
/*008c7e80*/ extern int g_gsd_load_8c7e80;
/*008c7e84*/ extern int g_gsd_load_8c7e84;
/*008c7e88*/ extern int g_data_8c7e88;
/*008c7e8c*/ extern int g_gsd_load_8c7e8c;
/*008c7e90*/ extern unsigned char g_gsd_load_8c7e90;
/*008c7e94*/ extern unsigned char g_gsd_load_8c7e94;
/*008c7e98*/ extern unsigned char g_gsd_load_8c7e98;
/*008c7e9c*/ extern int g_gsd_load_8c7e9c;
/*008c7ea0*/ extern int g_data_8c7ea0;
/*008c7ea4*/ extern int g_gsd_load_8c7ea4;
/*008c7ea8*/ extern int g_gsd_load_8c7ea8;
/* course_helper_449c40 */

#define SKY_PARTICLE_COUNT 0x100

/* a float stored and read back (the original keeps these temporaries in stack slots) */
static float float_barrier(float v) { return v; }

/* where new sky particles start: the emitter position and the size scale (read by the render code) */
typedef struct SkyEmitter {
  float x, y, z;
  float size;
} SkyEmitter;

typedef struct SkyParticle {
  float x, y, z;
  int age;
} SkyParticle;

typedef struct SkyVec {
  float x, y, z, pad;
} SkyVec;

extern SkyEmitter g_sky_emitter;                           /* 0x69d6d0 */
extern SkyParticle g_sky_particles[SKY_PARTICLE_COUNT];    /* 0x69d6e0 */
extern SkyVec g_sky_offsets[SKY_PARTICLE_COUNT];           /* 0x69e6e0 */
extern SkyVec g_sky_drift[SKY_PARTICLE_COUNT];             /* 0x69f6e0 */
extern unsigned char g_sky_colour_ramp[];                  /* 0x8c7ec0: alpha by age */

/* the locked vertex buffer of a PTank atomic: write pointer and stride */
typedef struct PTankLock {
  unsigned char *data;
  int stride;
} PTankLock;

/* "callers course_angels_cb1, course_keys_cb1, course_neustein_cb1, course_nt1_s_cb1: updates the sky particles of the course
   (drifting dust / stars): moves them, respawns the old ones around the emitter in front of the camera, writes positions and colours
   into the PTank buffers" */
void course_helper_449c40(RwMatrix *camera, float lead)
{
  PTankLock colour_lock;
  PTankLock position_lock;
  float spawn[3];
  float size;
  float speed;
  int respawned;
  int i;
  SkyParticle *p;
  float *vec;
  float step_x, step_y, step_z, s;
  double wind;   /* kept unrounded in the x87 register by the original */
  float wind_y, wind_z;
  int *atomic_data;
  float scale;

  speed = SUB_0044c2a0() * k_f_1_6666666;
  size = float_barrier(g_gsd_load_8c7e80 * k_f_0_1);
  spawn[0] = 0.0f;
  spawn[1] = 4.0f;
  spawn[2] = 0.0f;
  respawned = 0;
  if (g_gsd_load_8c7ea4 == 0) {
    return;
  }
  scale = (float)(speed * (g_gsd_load_8c7e8c * k_f_0_001));
  atomic_data = *(int **)(g_ptank_plugin_offset + (int)g_sky_atomic);
  *(float *)((char *)atomic_data + 0xb8) = scale;
  *(float *)((char *)atomic_data + 0xbc) = scale;
  atomic_data = *(int **)(g_ptank_plugin_offset + (int)g_sky_atomic);
  *(unsigned int *)((char *)atomic_data + 0x40) = *(unsigned int *)((char *)atomic_data + 0x40) | 0x4000;
  RpPTankAtomicLock_00545d10(g_sky_atomic, &position_lock, 1, 0x40000000);
  RpPTankAtomicLock_00545d10(g_sky_atomic, &colour_lock, 2, 0x40000000);
  if (g_gsd_load_8c7ea8 != 0) {
    lead = lead + k_f_12;
    spawn[0] = camera->at.x * lead + camera->pos.x;
    s = float_barrier(camera->at.y * lead);
    step_z = float_barrier(camera->at.z * lead);
    spawn[1] = (s + camera->pos.y) + k_f_4;
    spawn[2] = step_z + camera->pos.z;
  }
  g_sky_emitter.x = spawn[0];
  g_sky_emitter.y = spawn[1];
  g_sky_emitter.z = spawn[2];
  g_sky_emitter.size = size;
  if (position_lock.data != 0 && colour_lock.data != 0) {
    for (i = 0; i < SKY_PARTICLE_COUNT; i = i + 1) {
      p = &g_sky_particles[i];
      vec = SUB_00416850();
      step_x = float_barrier((SUB_00416860() * g_gsd_load_8c7e9c * k_f_0_001) * vec[0]);
      s = float_barrier(SUB_00416860() * g_gsd_load_8c7e9c * k_f_0_001);
      vec = SUB_00416850();
      step_y = float_barrier(vec[1] * s);
      s = float_barrier(SUB_00416860() * g_gsd_load_8c7e9c * k_f_0_001);
      vec = SUB_00416850();
      p->x = float_barrier(step_x + p->x);
      p->y = float_barrier(step_y + p->y);
      p->z = float_barrier(vec[2] * s + p->z);
      wind = g_data_8c7e88 * k_f_5_0en4;
      wind_y = float_barrier(wind * g_sky_drift[i].y);
      wind_z = float_barrier(wind * g_sky_drift[i].z);
      p->x = wind * g_sky_drift[i].x + p->x;
      p->y = wind_y + p->y;
      p->z = wind_z + p->z;
      if (g_gsd_load_8c7e84 < p->age) {
        p->age = respawned;
        respawned = respawned + 1;
        p->x = size * g_sky_offsets[i].x + spawn[0];
        p->y = size * g_sky_offsets[i].z + spawn[1];
        p->z = size * g_sky_offsets[i].y + spawn[2];
      }
      *(float *)position_lock.data = p->x;
      ((float *)position_lock.data)[1] = p->y;
      ((float *)position_lock.data)[2] = p->z;
      position_lock.data = position_lock.data + position_lock.stride;
      colour_lock.data[3] = g_sky_colour_ramp[(int)((float)p->age / (float)g_gsd_load_8c7e84 * g_data_8c7ea0)];
      colour_lock.data[0] = g_gsd_load_8c7e90;
      colour_lock.data[1] = g_gsd_load_8c7e94;
      colour_lock.data[2] = g_gsd_load_8c7e98;
      colour_lock.data = colour_lock.data + colour_lock.stride;
      p->age = p->age + 1;
    }
    RpPTankAtomicUnlock_00545f20(g_sky_atomic);
  }
  SUB_00416870(0.016666668f);
}

