// cluster-entries: course_highway_cb1_sub_453d00
/* Installed cluster course_highway_cb1_sub_453d00 (tools/cluster_install.py from output/cluster_pilot_kg/course_highway_cb1_sub_453d00.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004867f0*/ extern int particles_set_position(RwV3d *position);
/*00486820*/ extern int particles_set_size(float *size);
/*00486840*/ extern int particles_set_color(unsigned char *rgba);
/*00486b50*/ extern int particles_add(int pool);
/*00486c40*/ extern int particles_flush(int pool);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd338*/ extern float k_f_0_01;
/*005dd9ac*/ extern float k_f_255;
/*005ddb18*/ extern float float_0_8;
/*005de6a8*/ extern float k_f_n0_5;
/*005df50c*/ extern float k_f_n1_6;
/*005df510*/ extern float k_f_6_25en4;
/*005e64a4*/ extern float k_f_0;
/*006a1818*/ extern int g_course_6a1818;
/* course_highway_cb1_sub_453d00 */

/* single caller course_highway_cb1 */

#define HIGHWAY_OBJECT_COUNT 80

/* one highway object (0xfc bytes): the scene object it follows, the index into the scale table, and the active flag */
typedef struct HighwayObject {
  char *node;                 /* +0x00 (node + 4 -> the matrix of the object) */
  char _gap_4[8];
  int slot;                   /* +0x0c */
  char _gap_10[0x48];
  int active;                 /* +0x58 */
  char _gap_5c[0xa0];
} HighwayObject;

typedef struct HighwayScale {
  float side;                 /* +0 (8c3f60) */
  float up;                   /* +4 */
  float forward;              /* +8 */
} HighwayScale;

extern HighwayObject g_highway_objects[HIGHWAY_OBJECT_COUNT];   /* 0x8bf140 */
extern HighwayScale g_highway_scales[];                         /* 0x8c3f60 */

/* "for each active highway object: two sprites near the object, the alpha follows the angle between the direction to the player and the object's axis"; player_matrix = the player's matrix (position at +0x30) */
void course_highway_cb1_sub_453d00(int player_matrix)

{
  int slot;
  float scale_tmp;
  float side_offset;
  double dot;
  float *m;
  HighwayObject *object;
  struct {
    unsigned char colour[4];   /* 0xff, 0xff, 0xff, alpha */
    float forward;
    float side;
    float dist2;
    float up;
    float size_x;
    float size_y;
    RwV3d position;            /* the object's position, updated in place into the position of the sprites */
    RwV3d direction;           /* from the object to the player, normalized */
  } frame;

  for (object = g_highway_objects; object < g_highway_objects + HIGHWAY_OBJECT_COUNT; object++) {
    if (object->active != 0) {
      slot = object->slot;
      frame.side = g_highway_scales[slot].side;
      frame.up = g_highway_scales[slot].up;
      frame.forward = g_highway_scales[slot].forward;
      frame.size_x = 0.5f;
      frame.size_y = 0.5f;
      frame.colour[0] = 0xff;
      frame.colour[1] = 0xff;
      frame.colour[2] = 0xff;
      frame.colour[3] = 0xff;
      m = *(float **)(object->node + 4);
      frame.position.x = m[0x40 / 4];
      frame.position.y = m[0x44 / 4];
      frame.direction.x = *(float *)(player_matrix + 0x30) - frame.position.x;
      frame.position.z = m[0x48 / 4];
      frame.direction.y = *(float *)(player_matrix + 0x34) - frame.position.y;
      frame.direction.z = *(float *)(player_matrix + 0x38) - frame.position.z;
      dot = frame.direction.y * frame.direction.y;
      dot = dot + frame.direction.x * frame.direction.x;
      dot = dot + frame.direction.z * frame.direction.z;
      frame.dist2 = (float)dot;
      RwV3dNormalize(&frame.direction, &frame.direction);
      dot = frame.direction.y * m[0x34 / 4];
      dot = dot + frame.direction.x * m[0x30 / 4];
      dot = dot + frame.direction.z * m[0x38 / 4];
      if (k_f_0 >= dot) {
        dot = k_f_0;
      }
      scale_tmp = frame.dist2 * k_f_6_25en4;
      if (!(frame.size_x >= scale_tmp)) {
        frame.size_x = scale_tmp;
      }
      if (!(frame.size_y >= scale_tmp)) {
        frame.size_y = scale_tmp;
      }
      dot = dot * dot;
      frame.colour[3] = (unsigned char)(int)(dot * k_f_255);
      particles_set_size(&frame.size_x);
      particles_set_color(frame.colour);
      frame.position.x = frame.forward * m[0x30 / 4] + frame.position.x;
      frame.position.y = frame.forward * m[0x34 / 4] + frame.position.y;
      frame.position.z = frame.forward * m[0x38 / 4] + frame.position.z;
      frame.position.x = m[0x30 / 4] * k_f_0_01 + frame.position.x;
      frame.position.y = m[0x34 / 4] * k_f_0_01 + frame.position.y;
      frame.position.z = m[0x38 / 4] * k_f_0_01 + frame.position.z;
      scale_tmp = frame.up * k_f_n0_5;
      frame.position.x = m[0x20 / 4] * scale_tmp + frame.position.x;
      frame.position.y = m[0x24 / 4] * scale_tmp + frame.position.y;
      frame.position.z = m[0x28 / 4] * scale_tmp + frame.position.z;
      side_offset = frame.side * float_0_8;
      frame.position.x = side_offset * m[0x10 / 4] + frame.position.x;
      frame.position.y = side_offset * m[0x14 / 4] + frame.position.y;
      frame.position.z = side_offset * m[0x18 / 4] + frame.position.z;
      particles_set_position(&frame.position);
      particles_add(g_course_6a1818);
      scale_tmp = frame.side * k_f_n1_6;
      frame.position.x = scale_tmp * m[0x10 / 4] + frame.position.x;
      frame.position.y = scale_tmp * m[0x14 / 4] + frame.position.y;
      frame.position.z = scale_tmp * m[0x18 / 4] + frame.position.z;
      particles_set_position(&frame.position);
      particles_add(g_course_6a1818);
    }
  }
  particles_flush(g_course_6a1818);
  return;
}

