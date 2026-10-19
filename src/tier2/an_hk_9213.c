#include "mfl_tier2.h"
extern float s_333_Lap_frames_005df368;  /* mislabeled by the string finder: a float constant at 0x5df368 */
/*004726a0*/ extern int sfx_play_at();
/*00481fd0*/ extern double __cdecl random_range(float lo, float hi);
/*004820c0*/ extern int __cdecl colour_floats_to_bytes(unsigned char *out, float *a, float *b, float t);
/*0049d3d0*/ extern undefined __cdecl fx_smoke_puff_spawn(RwV3d *pos, unsigned char *colour, float vx, float vy, float vz);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*00555220*/ extern void * __cdecl RpMorphAtomicSetTime(void * p0, RwReal p1);
/*005dec48*/ extern float k_f_36;
/*005e0988*/ extern float k_f_1_6;
/*005e098c*/ extern float k_f_0_14;
/*0062ac0c*/ extern float g_event_log_62ac0c;
/*0062ac10*/ extern float g_event_log_62ac10;
/*0062ac74*/ extern float g_car_62ac74;
/*0076bfc0*/ extern undefined4 g_particles_76bfc0;
/*0076bfc4*/ extern undefined4 g_particles_76bfc4;
/*0076bfc8*/ extern undefined4 g_particles_76bfc8;
/*0076bfcc*/ extern undefined4 g_particles_76bfcc;
/*0076bfd0*/ extern undefined4 g_particles_76bfd0;
/*0076bfd4*/ extern undefined4 g_particles_76bfd4;
/*0076bfd8*/ extern undefined4 g_particles_76bfd8;
/*0076bfdc*/ extern undefined4 g_particles_76bfdc;
/*0076bfe0*/ extern undefined4 g_particles_76bfe0;
/*0076bfec*/ extern undefined4 g_load_76bfec;
/*0076dee0*/ extern undefined4 g_particles_76dee0;
/*0076df00*/ extern undefined4 g_event_log_76df00;
/* fx_explosion_spawn_ring */

/* "adds an explosion record (ring of 8) and spawns 15 smoke puffs around the position" */

void fx_explosion_spawn_ring(float *param_1,int param_2,undefined4 param_3)

{
  RwFrame *frame;
  int iVar3;
  char *rec;
  int *piVar4;
  int iVar5;
  float vx;
  float vy;
  float vz;
  unsigned char colour[4];
  RwV3d pos;

  iVar3 = g_event_log_76df00;
  rec = (char *)&g_particles_76bfc0 + iVar3 * 0xa4;
  (&g_particles_76dee0)[iVar3] = 1;
  g_event_log_76df00 = g_event_log_76df00 + 1;
  if ((int)g_event_log_76df00 >= 8) {
    g_event_log_76df00 = 0;
  }
  *(float *)rec = *param_1;
  *(float *)(rec + 4) = param_1[1];
  *(float *)(rec + 8) = param_1[2];
  *(int *)(rec + 0xc) = 0;
  *(int *)(rec + 0x10) = 0;
  *(int *)(rec + 0x20) = param_2;
  *(int *)(rec + 0x14) = 0;
  *(undefined4 *)(rec + 0x18) = param_3;
  *(int *)(rec + 0x1c) = 0;
  iVar5 = 0xf;
  do {
    pos.x = *param_1;
    pos.y = param_1[1];
    pos.z = param_1[2];
    pos.x = (float)(random_range(-0.05f,0.05f) + pos.x);
    pos.z = (float)(random_range(-0.05f,0.05f) + pos.z);
    vx = (float)(random_range(-0.1f,0.2f) + k_f_0_14);
    vy = (float)(random_range(-0.1f,0.1f) + k_f_1_6);
    vz = (float)(random_range(-0.1f,0.3f) + s_333_Lap_frames_005df368);
    colour_floats_to_bytes(colour,&g_event_log_62ac0c,&g_event_log_62ac10,(float)random_range(0.0f,0.2f));
    fx_smoke_puff_spawn(&pos,colour,vx,vy,vz);
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  *(int *)(rec + 0x1c) = 0x3e99999a;
  param_2 = 0;
  piVar4 = (int *)(rec + 0x2c);
  do {
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = -0.2f;
    frame = *(RwFrame **)(*piVar4 + 4);
    pos.z = (float)(random_range(0.7f,1.3f) * pos.z);
    RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,(float)param_2 * k_f_36,rwCOMBINEREPLACE);
    RwFrameTranslate(frame,&pos,rwCOMBINEPRECONCAT);
    RpMorphAtomicSetTime((void *)*piVar4,0);
    *(float *)&piVar4[10] = (float)random_range(0.7f,1.7f);
    param_2 = param_2 + 1;
    piVar4[0x14] = 0;
    piVar4 = piVar4 + 1;
  } while (param_2 < 10);
  sfx_play_at(0x1d,param_1);
  return;
}


