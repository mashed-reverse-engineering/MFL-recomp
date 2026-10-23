#include "mfl_tier2.h"
/*0040d0f0*/ extern char *__cdecl find_texture_dict_c(RwChar *p0);
/*00422350*/ extern int vehicle_part_ltm();
/*00435040*/ extern char *team_color_names();
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00886020*/ extern undefined4 g_app_886020[0x1000];
/*0088602c*/ extern undefined4 g_particles_88602c;
/*00886030*/ extern undefined4 g_effects_update_886030;
/*00886034*/ extern undefined4 g_effects_update_886034;
/*00886038*/ extern undefined4 g_effects_update_886038;
/*0088603c*/ extern undefined4 g_effects_update_88603c;
/*00886044*/ extern undefined4 g_effects_update_886044;
/*00886048*/ extern undefined g_effects_update_886048;
/*00886054*/ extern undefined4 g_effects_update_886054;
/*00886058*/ extern undefined4 g_effects_update_886058;
/*0088605c*/ extern undefined4 g_effects_update_88605c;
/*00886060*/ extern undefined g_effects_update_886060;
/*0088606c*/ extern undefined4 g_effects_update_88606c;
/* particle_emit_textured */

/* "tritex particle emitter" */

void __cdecl particle_emit_textured(float *emit_pos,undefined4 *default_tex_pair,int slot_or_tex,int mode,RwTexture *given_texture,
          float size_scale,float *extra_vel)

{
  PlayerSlot *player_slot;
  int *pool_entry;
  undefined4 part_id;
  RwTexture *texture;
  int pool_index;
  int pool_byte_off;
  float emit_y;
  double rand_val;
  struct {
    undefined1 team_col0_r;
    undefined1 team_col0_g;
    undefined1 team_col0_b;
    undefined1 team_col0_a;
    undefined1 team_col1_r;
    undefined1 team_col1_g;
    undefined1 team_col1_b;
    undefined1 team_col1_a;
    undefined1 team_col2_r;
    undefined1 team_col2_g;
    undefined1 team_col2_b;
    undefined1 team_col2_a;
    undefined1 team_col3_r;
    undefined1 team_col3_g;
    undefined1 team_col3_b;
    undefined1 team_col3_a;
    undefined1 team_col4_r;
    undefined1 team_col4_g;
    undefined1 team_col4_b;
    undefined1 team_col4_a;
    undefined1 team_col5_r;
    undefined1 team_col5_g;
    undefined1 team_col5_b;
    undefined1 team_col5_a;
    undefined4 white_tex_flags;
    float emit_x_or_size_x;
    float size_y;
    float emit_z_or_size_z;
    undefined1 *team_color_table [4];
    undefined1 *team_col4_ptr;
    undefined1 *team_col5_ptr;
  } frame;
  
  /* colour table of 6 team RGBA entries (indexed by the slot's team) */
  frame.team_col2_r = 0x3c;
  frame.team_col3_b = 0x3c;
  frame.team_color_table[0] = &frame.team_col0_r;
  frame.team_color_table[2] = &frame.team_col2_r;
  frame.team_col4_ptr = &frame.team_col4_r;
  frame.team_col0_a = 0xff;
  frame.team_col1_g = 0x54;
  frame.team_col1_a = 0xff;
  frame.team_col2_a = 0xff;
  frame.team_col3_a = 0xff;
  frame.team_col4_g = 0x54;
  frame.team_col4_a = 0xff;
  frame.team_col5_a = 0xff;
  frame.white_tex_flags = 0xffffffff;
  frame.team_color_table[1] = &frame.team_col1_r;
  frame.team_color_table[3] = &frame.team_col3_r;
  player_slot = g_player_slots + slot_or_tex;
  pool_index = player_slot->team;
  frame.team_col5_ptr = &frame.team_col5_r;
  frame.team_col0_r = 0x78;
  frame.team_col0_g = 0x19;
  frame.team_col0_b = 0x1e;
  frame.team_col1_r = 4;
  frame.team_col1_b = 0xa4;
  frame.team_col2_g = 0x7c;
  frame.team_col2_b = 0x4c;
  frame.team_col3_r = 0xe4;
  frame.team_col3_g = 0xc4;
  frame.team_col4_r = 0xd4;
  frame.team_col4_b = 0x9c;
  frame.team_col5_r = 0;
  frame.team_col5_g = 0;
  frame.team_col5_b = 0;
  if (mode == 1) {
    part_id = 0x33;
  }
  else {
    if (mode != 2) {
      frame.emit_x_or_size_x = *emit_pos;
      emit_y = emit_pos[1];
      frame.emit_z_or_size_z = emit_pos[2];
      slot_or_tex = *default_tex_pair;
      goto LAB_004a0037;
    }
    part_id = 0x34;
  }
  /* mode 1 / 2: emit from the car part 0x33 / 0x34 (position from its matrix) */
  pool_byte_off = vehicle_part_ltm(slot_or_tex,part_id);
  frame.emit_x_or_size_x = *(float *)(pool_byte_off + 0x30);
  emit_y = *(float *)(pool_byte_off + 0x34);
  frame.emit_z_or_size_z = *(float *)(pool_byte_off + 0x38);
  slot_or_tex = *(int *)frame.team_color_table[pool_index];
LAB_004a0037:
  /* find a free entry of the particle pool (80 byte records at g_app_886020); give up when full */
  pool_index = 0;
  pool_entry = g_app_886020;
  while (*pool_entry != 0) {
    pool_entry = pool_entry + 0x14;
    pool_index = pool_index + 1;
    if ((char *)&g_effects_update_88606c + 0x913 < (char *)pool_entry) {
      return;
    }
  }
  rand_val = (double)random_range(0xba83126f,0x3a83126f);
  frame.emit_x_or_size_x = (float)(rand_val + (double)frame.emit_x_or_size_x);
  rand_val = (double)random_range(0xba83126f,0x3a83126f);
  pool_byte_off = pool_index * 0x50;
  frame.emit_z_or_size_z = (float)(rand_val + (double)frame.emit_z_or_size_z);
  /* fill the entry: jittered position, random life and velocity, size = size_scale * random */
  *(float *)&(&g_effects_update_886030)[pool_index * 0x14] = frame.emit_x_or_size_x;
  *(float *)&(&g_effects_update_886034)[pool_index * 0x14] = emit_y;
  *(float *)&(&g_effects_update_886038)[pool_index * 0x14] = frame.emit_z_or_size_z;
  (&g_effects_update_88605c)[pool_index * 0x14] = 0;
  part_id = random_int_range(0x14,0x23);
  (&g_effects_update_886058)[pool_index * 0x14] = part_id;
  rand_val = (double)random_range(0,0x3f800000);
  *(float *)((char *)&g_effects_update_886060 + pool_byte_off) = (float)rand_val;
  rand_val = (double)random_range(0,0x3f800000);
  *(float *)(pool_byte_off + ((char *)&g_effects_update_886060 + 0x4)) = (float)rand_val;
  rand_val = (double)random_range(0,0x3f800000);
  *(float *)(pool_byte_off + ((char *)&g_effects_update_886060 + 0x8)) = (float)rand_val;
  rand_val = (double)random_range(0xbcf5c28f,0x3cf5c28f);
  *(float *)&(&g_effects_update_88603c)[pool_index * 0x14] = (float)rand_val;
  rand_val = (double)random_range(0xbcf5c28f,0x3cf5c28f);
  *(float *)&(&g_effects_update_886044)[pool_index * 0x14] = (float)rand_val;
  rand_val = (double)random_range(0x3c23d70a,0x3d23d70a);
  *(float *)(pool_byte_off + ((char *)&g_effects_update_88603c + 0x4)) = (float)rand_val;
  if (extra_vel != (float *)0x0) {
    *(float *)&(&g_effects_update_88603c)[pool_index * 0x14] =
         *(float *)&(&g_effects_update_88603c)[pool_index * 0x14] + *extra_vel;
    *(float *)(pool_byte_off + ((char *)&g_effects_update_88603c + 0x4)) = (float)((double)extra_vel[1] + rand_val);
    *(float *)&(&g_effects_update_886044)[pool_index * 0x14] =
         extra_vel[2] + *(float *)&(&g_effects_update_886044)[pool_index * 0x14];
  }
  (&g_effects_update_88606c)[pool_index * 0x14] = 0;
  (&g_effects_update_886054)[pool_index * 0x14] = 0;
  rand_val = (double)random_range(0x3dcccccd,0x3f19999a);
  frame.emit_x_or_size_x = (float)(rand_val * (double)size_scale);
  rand_val = (double)random_range(0x3dcccccd,0x3f19999a);
  frame.size_y = (float)(rand_val * (double)size_scale);
  rand_val = (double)random_range(0x3dcccccd,0x3f19999a);
  *(float *)((char *)&g_effects_update_886048 + pool_byte_off) = frame.emit_x_or_size_x;
  frame.emit_z_or_size_z = (float)(rand_val * (double)size_scale);
  *(float *)(pool_byte_off + ((char *)&g_effects_update_886048 + 0x4)) = frame.size_y;
  *(float *)(pool_byte_off + ((char *)&g_effects_update_886048 + 0x8)) = frame.emit_z_or_size_z;
  (g_app_886020)[pool_index * 0x14] = 1;
  /* texture: the given one, else a team texture (1 in 4 for mode 1) or "tritex" */
  if (given_texture == (RwTexture *)0x0) {
    if ((mode == 1) && (pool_byte_off = random_int_range(0,3), pool_byte_off == 3)) {
      texture = (RwTexture *)team_color_names(player_slot->team);
    }
    else {
      texture = (RwTexture *)find_texture_dict_c("tritex");
    }
    RpMaterialSetTexture((RpMaterial *)(&g_particles_88602c)[pool_index * 0x14],texture);
    *(int *)((&g_particles_88602c)[pool_index * 0x14] + 4) = slot_or_tex;
    return;
  }
  RpMaterialSetTexture((RpMaterial *)(&g_particles_88602c)[pool_index * 0x14],given_texture);
  *(undefined4 *)((&g_particles_88602c)[pool_index * 0x14] + 4) = frame.white_tex_flags;
  return;
}


