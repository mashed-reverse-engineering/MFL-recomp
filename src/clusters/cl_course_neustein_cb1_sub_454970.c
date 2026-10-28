// cluster-entries: course_neustein_cb1_sub_454970
/* Installed cluster course_neustein_cb1_sub_454970 (tools/cluster_install.py from output/cluster_pilot_md/course_neustein_cb1_sub_454970.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040fbf0*/ extern void __cdecl race_get_focus_player(int *player, int *camera);
/*00408ea0*/ extern int __cdecl player_record_get_8d8ca0(int player);
/*0048e4f0*/ extern void __cdecl rwp_object_set_enabled(int handle, int enabled);
/*00492d10*/ extern void __cdecl rwp_object_set_active(int handle, int active);
/*0048e5a0*/ extern void __cdecl course_neustein_cb1_sub_48e5a0(int handle, int on);
/*0048e6f0*/ extern int __cdecl rwp_object_get_matrix(int handle);
/*0048e720*/ extern void __cdecl rwp_object_set_matrix(int handle, RwMatrix *matrix);
/*004548b0*/ extern void __cdecl course_neustein_cb1_sub_4548b0(float a, float b);
/*004c48b0*/ extern void __cdecl hanim_keyframes_sample_at_time(RwMatrix *out, void *path, float time);
/*0056b1d0*/ extern void __cdecl FUN_0056b1d0(int world, int owner, int enable);
/*0058c8a0*/ extern int __cdecl FUN_0058c8a0(int handle);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*0042b110*/ extern int current_course_id();
/*005dd4d0*/ extern float k_f_0;
/*005decd4*/ extern float k_f_65;
/*005df52c*/ extern float k_f_n65;
/*005df528*/ extern float k_f_1_8;
/*005df524*/ extern float k_f_100000;
/*005de3a0*/ extern float k_f_1_3;
/*007ef7f4*/ extern float g_tick_seconds;
/*0060d8b0*/ extern undefined g_course_60d8b0;
/*006a1d98*/ extern int g_data_6a1d98;
/*006a1d9c*/ extern int g_data_6a1d9c;
/*006a1cc8*/ extern float g_neustein_corners[16][3];
/* course_neustein_cb1_sub_454970 */

#define COURSE_NEUST_R (5)
#define HULL_COUNT 2
#define CORNERS_PER_HULL 8

/* the two moving hulls of the Neustein course (0x28 bytes each) */
typedef struct NeusteinHull {
  int body_a;       /* +0x00 rwp body ids, resolved with FUN_0058c8a0 */
  int body_b;       /* +0x04 */
  int unused_8;
  void *path;       /* +0x0c motion path (duration at +0x0c, evaluated by hanim_keyframes_sample_at_time) */
  int object_a;     /* +0x10 rwp object handles that follow the path */
  int object_b;     /* +0x14 */
  float time;       /* +0x18 position on the path */
  int unused_1c;
  int corner_a;     /* +0x20 corner nearest to object b */
  int corner_b;     /* +0x24 the corner next to it that lies away from the other neighbour */
} NeusteinHull;

extern NeusteinHull g_neustein_state[HULL_COUNT];   /* 0x6a1878 */

/* "single caller course_neustein_cb1: while the focus player is on the hull section of the course, moves the two hulls along their paths,
   aims the dust particles at them and tracks the corner nearest to the player's hull" */
void course_neustein_cb1_sub_454970(void)
{
  RwV3d lift;
  RwV3d d_a;
  RwV3d d_b;
  RwMatrix path_matrix;
  int player;
  int camera;
  int record;
  int hull_index;
  int corner_base;
  int i;
  int idx;
  int next;
  int prev;
  NeusteinHull *hull;
  float *pos;
  float dsq;
  float best;
  float angle;
  float side;
  int course;
  int body_a_owner;
  int body_b_owner;
  int *frame_a;
  int *frame_b;
  int obj_matrix;
  float dx, dy, dz;
  float *vertex;
  int active;

  lift.x = 0.0f;
  lift.y = 0.65f;
  lift.z = 0.0f;
  g_data_6a1d9c = g_data_6a1d98;
  g_data_6a1d98 = 0;
  race_get_focus_player(&player, &camera);
  record = player_record_get_8d8ca0(player);
  course = current_course_id();
  active = 0;
  if (course == COURSE_NEUST_R) {
    if ((0 <= record && record < 0x10) || (0x44 <= record && record <= 0x47)) {
      active = 1;
    }
  }
  else if (0x2d <= record && record <= 0x46) {
    active = 1;
  }
  if (active) {
    g_data_6a1d98 = 1;
  }
  if (g_data_6a1d98 == 0) {
    /* off the hull section: both objects of both hulls are switched off */
    for (hull_index = 0; hull_index < HULL_COUNT; hull_index = hull_index + 1) {
      hull = &g_neustein_state[hull_index];
      rwp_object_set_enabled(hull->object_a, 0);
      course_neustein_cb1_sub_48e5a0(hull->object_a, 0);
      rwp_object_set_active(hull->object_a, 0);
      rwp_object_set_enabled(hull->object_b, 0);
      course_neustein_cb1_sub_48e5a0(hull->object_b, 0);
      rwp_object_set_active(hull->object_b, 0);
    }
    return;
  }
  if (g_data_6a1d98 != g_data_6a1d9c) {
    course_neustein_cb1_sub_4548b0(0.0f, 0.0f);
  }
  corner_base = 0;
  for (hull_index = 0; hull_index < HULL_COUNT; hull_index = hull_index + 1) {
    hull = &g_neustein_state[hull_index];
    body_b_owner = FUN_0058c8a0(hull->body_b);
    body_a_owner = FUN_0058c8a0(hull->body_a);
    side = 0.0f;
    course_neustein_cb1_sub_48e5a0(hull->object_a, 1);
    rwp_object_set_enabled(hull->object_a, 1);
    rwp_object_set_active(hull->object_a, 1);
    course_neustein_cb1_sub_48e5a0(hull->object_b, 1);
    rwp_object_set_enabled(hull->object_b, 1);
    rwp_object_set_active(hull->object_b, 1);
    frame_a = *(int **)(*(int *)(body_a_owner + 0x10) + 4);
    frame_b = *(int **)(*(int *)(body_b_owner + 0x10) + 4);
    if (*(float *)((char *)hull->path + 0xc) <= hull->time) {
      hull->time = 0.0f;
      hanim_keyframes_sample_at_time(&path_matrix, hull->path, 0.0f);
      rwp_object_set_matrix(hull->object_b, &path_matrix);
      RwMatrixTranslate(&path_matrix, (RwV3d *)&g_course_60d8b0, rwCOMBINEPOSTCONCAT);
      rwp_object_set_matrix(hull->object_a, &path_matrix);
    }
    hanim_keyframes_sample_at_time(&path_matrix, hull->path, hull->time);
    RwMatrixTranslate(&path_matrix, &lift, rwCOMBINEPOSTCONCAT);
    obj_matrix = rwp_object_get_matrix(hull->object_b);
    dx = path_matrix.pos.x - *(float *)(obj_matrix + 0x30);
    dy = path_matrix.pos.y - *(float *)(obj_matrix + 0x34);
    dz = path_matrix.pos.z - *(float *)(obj_matrix + 0x38);
    FUN_0056b1d0(*(int *)(body_b_owner + 0x24), body_b_owner, 1);
    rwp_object_set_enabled(hull->object_b, 1);
    if (frame_b != 0) {
      vertex = (float *)(*(int *)(*(int *)(*frame_b + 0x10) + 8) + 0x10 + frame_b[1] * 0x20);
      side = atan2(atan2(path_matrix.at.z, path_matrix.at.x), 1.0) - atan2(atan2(*(float *)(obj_matrix + 0x28), *(float *)(obj_matrix + 0x20)), 1.0);
      angle = k_f_65;
      if (*(float *)(obj_matrix + 0x28) < k_f_0) {
        angle = k_f_n65;
      }
      vertex[0] = (*(float *)(obj_matrix + 0x24) - path_matrix.at.y) * angle;
      vertex[1] = -side;
      dx = dx * k_f_1_8;
      dy = dy * k_f_1_8;
      dz = dz * k_f_1_8;
      vertex = (float *)(*(int *)(*(int *)(*frame_b + 0x10) + 8) + frame_b[1] * 0x20);
      vertex[0] = dx;
      vertex[1] = dy;
      vertex[2] = dz;
    }
    if (frame_a != 0) {
      vertex = (float *)(*(int *)(*(int *)(*frame_a + 0x10) + 8) + 0x10 + frame_a[1] * 0x20);
      vertex[1] = -side;
    }
    /* nearest corner of this hull to the object, then the corner next to it away from the other neighbour */
    pos = (float *)(obj_matrix + 0x30);
    idx = -1;
    best = k_f_100000;
    for (i = 0; i < CORNERS_PER_HULL; i = i + 1) {
      float *c = g_neustein_corners[corner_base + i];
      dx = c[0] - pos[0];
      dy = c[1] - pos[1];
      dz = c[2] - pos[2];
      dsq = (dy * dy + dx * dx) + dz * dz;
      if (dsq < best) {
        idx = i;
        hull->corner_a = i;
        best = dsq;
      }
    }
    next = idx + 1;
    prev = idx - 1;
    if (prev < 0) {
      prev = 0;
    }
    if (next >= CORNERS_PER_HULL) {
      next = CORNERS_PER_HULL - 1;
    }
    d_a.x = g_neustein_corners[idx + corner_base][0] - pos[0];
    d_a.y = g_neustein_corners[idx + corner_base][1] - pos[1];
    d_a.z = g_neustein_corners[idx + corner_base][2] - pos[2];
    d_b.x = g_neustein_corners[next + corner_base][0] - pos[0];
    d_b.y = g_neustein_corners[next + corner_base][1] - pos[1];
    d_b.z = g_neustein_corners[next + corner_base][2] - pos[2];
    RwV3dNormalize(&d_a, &d_a);
    RwV3dNormalize(&d_b, &d_b);
    if (!(d_b.z * d_a.z + d_b.x * d_a.x + d_b.y * d_a.y < k_f_0)) {
      next = prev;
    }
    hull->corner_b = next;
    hull->time = g_tick_seconds * k_f_1_3 + hull->time;
    corner_base = corner_base + CORNERS_PER_HULL;
  }
  course = current_course_id();
  if (course == COURSE_NEUST_R ? record == 0x44 : record == 0x37) {
    course_neustein_cb1_sub_4548b0(23.0f, 0.8f);
  }
}

