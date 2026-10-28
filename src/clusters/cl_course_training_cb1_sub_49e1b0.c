// cluster-entries: course_training_cb1_sub_49e1b0
/* Installed cluster course_training_cb1_sub_49e1b0 (tools/cluster_install.py from output/cluster_pilot_k/course_training_cb1_sub_49e1b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042a460*/ extern int course_get_light_position();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0049a960*/ extern int RpPTankAtomicLock_0049a960(); /* import thunk of RpPTankAtomicLock_00545d10: the original calls it for the first three locks and the function itself for the last */
/*00545d10*/ extern int RpPTankAtomicLock_direct();
/*0049a970*/ extern int RpPTankAtomicUnlock_0049a970();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005dec68*/ extern float k_f_0_03125;
/*0062ac14*/ extern undefined4 g_data_62ac14;
/*0062ac18*/ extern float g_data_62ac18;
/*0062ac1c*/ extern float g_data_62ac1c;
/*0076df18*/ extern undefined4 g_fx_ptank_76df18;
/*0076df28*/ extern undefined4 g_fx_ptank_76df28;
/*0076df2c*/ extern undefined4 g_fx_ptank_76df2c;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*00886980*/ extern float g_view_pos_x;
/*00886984*/ extern float g_view_pos_y;
/*00886988*/ extern float g_view_pos_z;
/* course_training_cb1_sub_49e1b0 */

/* single caller course_training_cb1 */

/* RpPTank lock record: base pointer of one attribute stream and its stride in bytes */
typedef struct PTankLock { unsigned char *data; int stride; } PTankLock;

#define RP_PTANK_LOCK_POSITION 0x01
#define RP_PTANK_LOCK_COLOR    0x02
#define RP_PTANK_LOCK_SIZE     0x04
#define RP_PTANK_LOCK_MATRIX   0x80 /* flag 0x80: 4 floats per particle, used here as texture rect (u, v, w, h) */
#define SPRAY_PARTICLES 32
#define SPRAY_STEP_DECAY 0.03125f
#define PTANK_PLUGIN_FLAGS_OFFSET 0x40 /* flag word inside the PTank atomic plugin data */

/* course_training_cb1_sub_49e1b0 */

/*
 * Rebuilds the particle stream of the training course's light beam ("spray"): SPRAY_PARTICLES
 * particles on a ray from the light source (or the first scene light), spaced by a step that
 * shrinks along the ray, with a colour ramp (first two fixed, then the 8 entry colour table halved),
 * a size that fades out and the lit amount light_scale (squared facing of the light).
 * beam_owner: the owner object (+4 = scene node whose matrix gives the ray basis).
 */
void course_training_cb1_sub_49e1b0(int beam_owner)

{
  double facing;
  double clamped;
  float light_scale;
  float *light;
  int scene;
  int view_matrix;
  unsigned int *flags;
  PTankLock pos;
  PTankLock color;
  PTankLock size;
  PTankLock tex;
  RwV3d source_pos;
  RwV3d light_dir;
  RwV3d target_pos;
  float alpha_scale[4] = {0.05f, 0.1f, 0.2f, 0.15f};
  unsigned char colour_table[8][4] = {
    {0xff, 0x00, 0x00, 0xff},
    {0xff, 0x80, 0x00, 0xff},
    {0xff, 0xff, 0x00, 0xff},
    {0x00, 0xff, 0x00, 0xff},
    {0x00, 0x00, 0xff, 0xff},
    {0x80, 0x00, 0xff, 0xff},
    {0xff, 0x00, 0xff, 0xff},
    {0xff, 0x00, 0x00, 0xff}
  };
  int i;
  int alpha;
  double step;
  double t;
  float step0;
  float size_value;
  float tex_u;
  float tex_v;
  float tex_w;
  float tex_h;
  const unsigned char *colour;

  light = (float *)course_get_light_position();
  light_dir.x = *light;
  light_dir.y = light[1];
  scene = *(int *)(beam_owner + 4);
  light_dir.z = light[2];
  view_matrix = get_viewport_camera_matrix(0);
  g_view_pos_x = *(float *)(view_matrix + 0x30);
  g_view_pos_y = *(float *)(view_matrix + 0x34);
  g_view_pos_z = *(float *)(view_matrix + 0x38);
  if (g_fx_ptank_76df2c != 0) {
    light_dir.x = *(float *)(g_fx_ptank_76df2c + 0x20);
    light_dir.y = *(float *)(g_fx_ptank_76df2c + 0x24);
    light_dir.z = *(float *)(g_fx_ptank_76df2c + 0x28);
    source_pos.x = *(float *)(g_fx_ptank_76df2c + 0x30);
    source_pos.y = *(float *)(g_fx_ptank_76df2c + 0x34);
    source_pos.z = *(float *)(g_fx_ptank_76df2c + 0x38);
  }
  target_pos.x = g_view_pos_x - source_pos.x;
  target_pos.y = g_view_pos_y - source_pos.y;
  target_pos.z = g_view_pos_z - source_pos.z;
  RwV3dLength(&target_pos);
  target_pos.x = g_data_62ac1c * *(float *)(scene + 0x30) + *(float *)(scene + PTANK_PLUGIN_FLAGS_OFFSET);
  target_pos.y = g_data_62ac1c * *(float *)(scene + 0x34) + *(float *)(scene + 0x44);
  target_pos.z = g_data_62ac1c * *(float *)(scene + 0x38) + *(float *)(scene + 0x48);
  /* how much the light faces the scene node: the x87 keeps the whole chain in extended precision, only the final square is stored as a float */
  facing = (light_dir.z * *(float *)(scene + 0x38) +
           light_dir.x * *(float *)(scene + 0x30) + light_dir.y * *(float *)(scene + 0x34)) * k_f_n1;
  clamped = k_f_0;
  if (!(k_f_0 > facing)) {
    clamped = facing;
    if (float_1_0 < facing)
      clamped = float_1_0;
  }
  light_scale = (float)(clamped * clamped);
  RpPTankAtomicLock_0049a960(g_fx_ptank_76df18,&pos,RP_PTANK_LOCK_POSITION,0x40000000);
  RpPTankAtomicLock_0049a960(g_fx_ptank_76df18,&color,RP_PTANK_LOCK_COLOR,0x40000000);
  RpPTankAtomicLock_0049a960(g_fx_ptank_76df18,&size,RP_PTANK_LOCK_SIZE,0x40000000);
  RpPTankAtomicLock_direct(g_fx_ptank_76df18,&tex,RP_PTANK_LOCK_MATRIX,0x40000000);
  if (pos.data != 0 && color.data != 0 && size.data != 0) {
    if (g_fx_ptank_76df2c == 0) {
      source_pos.x = source_pos.x + g_view_pos_x;
      source_pos.y = source_pos.y + g_view_pos_y;
      source_pos.z = source_pos.z + g_view_pos_z;
    }
    light_dir.x = target_pos.x - source_pos.x;
    light_dir.y = target_pos.y - source_pos.y;
    light_dir.z = target_pos.z - source_pos.z;
    RwV3dNormalize(&light_dir,&light_dir);
    RwFrameTranslate(g_fx_ptank_76df28,&source_pos,rwCOMBINEREPLACE);
    alpha = g_data_62ac14;
    step0 = g_data_62ac18;
    step = step0;
    if (alpha > 0x80)
      alpha = 0x80;
    for (i = 0; i < SPRAY_PARTICLES; i++) {
      /* position: along the ray, the index lags one behind (particle 0 and 1 share the source) */
      t = (float)(i == 0 ? 0 : i - 1) * step;
      ((float *)pos.data)[0] = light_dir.x * t + source_pos.x;
      ((float *)pos.data)[1] = light_dir.y * t + source_pos.y;
      ((float *)pos.data)[2] = t * light_dir.z + source_pos.z;
      pos.data += pos.stride;
      tex_u = k_f_0_5;
      tex_v = k_f_0;
      if (i == 0) {
        color.data[0] = 0xff;
        size_value = light_scale * k_f_4;
        color.data[1] = 0xc0;
        tex_w = 1.0f;
        tex_h = 0.5f;
        color.data[2] = 0x80;
      } else if (i == 1) {
        tex_w = 0.5f;
        tex_h = 0.5f;
        size_value = light_scale * k_f_4;
        color.data[0] = 0xff;
        step = step - step0 * k_f_0_03125;
        color.data[1] = 0xdc;
        color.data[2] = 0xa0;
        tex_u = tex_v;
      } else {
        step = step - step0 * k_f_0_03125;
        tex_w = 1.0f;
        tex_h = 1.0f;
        colour = colour_table[i & 7];
        size_value = (k_f_8 - (float)(i - 2) * k_f_0_125) * alpha_scale[i & 3] * light_scale;
        color.data[0] = colour[0] >> 1;
        color.data[1] = colour[1] >> 1;
        color.data[2] = colour[2] >> 1;
        tex_v = tex_u;
      }
      color.data[3] = (unsigned char)alpha;
      color.data += color.stride;
      ((float *)size.data)[0] = size_value;
      ((float *)size.data)[1] = size_value;
      size.data += size.stride;
      ((float *)tex.data)[0] = tex_u;
      ((float *)tex.data)[1] = tex_v;
      ((float *)tex.data)[2] = tex_w;
      ((float *)tex.data)[3] = tex_h;
      tex.data += tex.stride;
    }
    RpPTankAtomicUnlock_0049a970(g_fx_ptank_76df18);
  }
  flags = (unsigned int *)(*(int *)(g_ptank_plugin_offset + g_fx_ptank_76df18) + PTANK_PLUGIN_FLAGS_OFFSET);
  *flags = *flags | 0x800000;
  *(unsigned int *)(*(int *)(g_ptank_plugin_offset + g_fx_ptank_76df18) + 4) = SPRAY_PARTICLES;
  scene = *(int *)(beam_owner + 4);
  g_view_pos_x = *(float *)(scene + PTANK_PLUGIN_FLAGS_OFFSET);
  g_view_pos_y = *(float *)(scene + 0x44);
  g_view_pos_z = *(float *)(scene + 0x48);
  return;
}

