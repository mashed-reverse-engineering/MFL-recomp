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

void fx_explosion_spawn_ring(float *pos_in,int ring_arg_a,undefined4 ring_arg_b)

{
  RwFrame *frame;
  int ring_index;
  char *rec;
  int *piece_cursor;
  int puffs_left;
  float vx;
  float vy;
  float vz;
  unsigned char colour[4];
  RwV3d pos;

  ring_index = g_event_log_76df00;
  rec = (char *)&g_particles_76bfc0 + ring_index * 0xa4;
  (&g_particles_76dee0)[ring_index] = 1;
  g_event_log_76df00 = g_event_log_76df00 + 1;
  if ((int)g_event_log_76df00 >= 8) {
    g_event_log_76df00 = 0;
  }
  *(float *)rec = *pos_in;
  *(float *)(rec + 4) = pos_in[1];
  *(float *)(rec + 8) = pos_in[2];
  *(int *)(rec + 0xc) = 0;
  *(int *)(rec + 0x10) = 0;
  *(int *)(rec + 0x20) = ring_arg_a;
  *(int *)(rec + 0x14) = 0;
  *(undefined4 *)(rec + 0x18) = ring_arg_b;
  *(int *)(rec + 0x1c) = 0;
  puffs_left = 0xf;
  do {
    pos.x = *pos_in;
    pos.y = pos_in[1];
    pos.z = pos_in[2];
    pos.x = (float)(random_range(-0.05f,0.05f) + pos.x);
    pos.z = (float)(random_range(-0.05f,0.05f) + pos.z);
    vx = (float)(random_range(-0.1f,0.2f) + k_f_0_14);
    vy = (float)(random_range(-0.1f,0.1f) + k_f_1_6);
    vz = (float)(random_range(-0.1f,0.3f) + s_333_Lap_frames_005df368);
    colour_floats_to_bytes(colour,&g_event_log_62ac0c,&g_event_log_62ac10,(float)random_range(0.0f,0.2f));
    fx_smoke_puff_spawn(&pos,colour,vx,vy,vz);
    puffs_left = puffs_left + -1;
  } while (puffs_left != 0);
  *(int *)(rec + 0x1c) = 0x3e99999a;
  ring_arg_a = 0;
  piece_cursor = (int *)(rec + 0x2c);
  do {
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = -0.2f;
    frame = *(RwFrame **)(*piece_cursor + 4);
    pos.z = (float)(random_range(0.7f,1.3f) * pos.z);
    RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,(float)ring_arg_a * k_f_36,rwCOMBINEREPLACE);
    RwFrameTranslate(frame,&pos,rwCOMBINEPRECONCAT);
    RpMorphAtomicSetTime((void *)*piece_cursor,0);
    *(float *)&piece_cursor[10] = (float)random_range(0.7f,1.7f);
    ring_arg_a = ring_arg_a + 1;
    piece_cursor[0x14] = 0;
    piece_cursor = piece_cursor + 1;
  } while (ring_arg_a < 10);
  sfx_play_at(0x1d,pos_in);
  return;
}


