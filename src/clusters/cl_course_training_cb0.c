// cluster-entries: course_training_cb0
/* Installed cluster course_training_cb0 (tools/cluster_install.py from output/cluster_pilot_kb/course_training_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char UNK_005df51c[];
extern int _rpUVAnimDictSchema;
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004849e0*/ extern int course_helper_4849e0();
/*00485180*/ extern int course_training_cb0_sub_485180();
/*0049c1a0*/ extern int particles_slot_set_vectors();
/*0049c280*/ extern int fx_snow_ptank_create();
/*0049e170*/ extern int course_training_cb0_sub_49e170(int unused); /* the caller pushes one argument that the function does not read */
/*00569270*/ extern int RtDictSchemaSetCurrentDict();
/*005dd340*/ extern float k_f_0_5;
/*0064d894*/ extern undefined4 g_course_64d894;
/*008bf080*/ extern undefined4 g_course_8bf080;
/*008bf084*/ extern undefined4 g_course_training_8bf084;
/* course_training_cb0 */


/* Training-course setup callback: creates the smoke particle system, then for each of the course's entries (count at course+0xc008, stride 0x40 bytes from course+0x1830) places one particle slot at the entry position (y lowered by 0.5); slot index advances after the first 4 entries. vec_a / vec_b are two constant 3-vectors passed to particles_slot_set_vectors (inferred meaning). */
void course_training_cb0(char *course)

{
  undefined4 texture_dict;
  int slot_index;
  int sprite_index;
  undefined4 *sprite_src;
  struct {
    undefined4 pos_x;
    float pos_y;
    undefined4 pos_z;
    undefined4 vec_b_x;
    undefined4 vec_b_y;
    undefined4 vec_b_z;
    undefined4 vec_a_x;
    undefined4 vec_a_y;
    undefined4 vec_a_z;
  } frame;
  
  course_training_cb0_sub_49e170(0);
  g_course_training_8bf084 = course_helper_4849e0(&UNK_005df51c,*(undefined4 *)(course + 0xc9d0));
  frame.pos_x = 0xc23af5c3;
  frame.pos_y = 0.747;
  frame.pos_z = 0xc28c245a;
  frame.vec_a_x = 0xbbf5c28f;
  frame.vec_a_y = 0x3c75c28f;
  frame.vec_a_z = 0xbbf5c28f;
  frame.vec_b_x = 0x3bf5c28f;
  frame.vec_b_y = 0x3d23d70a;
  frame.vec_b_z = 0x3bf5c28f;
  texture_dict = find_texture_dict_b("smoke");
  fx_snow_ptank_create(0,texture_dict);
  slot_index = 0;
  sprite_index = 0;
  if (0 < *(int *)(course + 0xc008)) {
    sprite_src = (undefined4 *)(course + 0x1830);
    do {
      frame.pos_x = *sprite_src;
      frame.pos_z = sprite_src[2];
      frame.pos_y = *(float *)&sprite_src[1] - k_f_0_5;
      particles_slot_set_vectors(slot_index,&frame.pos_x,0x3a83126f,&frame.vec_a_x,&frame.vec_b_x);
      if (3 < sprite_index) {
        slot_index = slot_index + 1;
      }
      sprite_index = sprite_index + 1;
      sprite_src = sprite_src + 0x10;
    } while (sprite_index < *(int *)(course + 0xc008));
  }
  g_course_8bf080 = *(undefined4 *)(course + 0xca18);
  RtDictSchemaSetCurrentDict(&_rpUVAnimDictSchema,g_course_8bf080);
  if (g_course_64d894 != 0) {
    course_training_cb0_sub_485180(*(undefined4 *)(course + 0xc13c));
  }
  return;
}

