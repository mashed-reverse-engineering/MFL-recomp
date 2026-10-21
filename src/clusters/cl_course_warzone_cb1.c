// cluster-entries: course_warzone_cb1
/* Installed cluster course_warzone_cb1 (tools/cluster_install.py from output/cluster_pilot_uc3/course_warzone_cb1.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern int g_course_warzone_8bf044;
extern int g_course_warzone_6a27a0;
extern int g_course_warzone_8bf060[8];
extern int g_course_60d9c8;
extern int g_course_60d9cc;
extern char g_course_6a2888[];
extern RwMatrix g_warzone_matrix_a;
extern RwMatrix g_warzone_matrix_b;
/*0045c9c0*/ extern void camera_shake_decay(float dt);
/*00475730*/ extern RwCamera *get_viewport_camera(int index);
/*0048e180*/ extern void ptank_lock_all(RwCamera *camera);
/*0048e720*/ extern void rwp_object_set_matrix(int object, RwMatrix *matrix);
/*0048e6d0*/ extern int rwp_object_get_handle(int object);
/*0058c8a0*/ extern int FUN_0058c8a0(int handle);
extern void SUB_0045d720(int a, int b, float dt);
extern void SUB_0045e0f0(char *scene);
extern void SUB_0045d1b0(char *obj, int elapsed_ms);
extern void SUB_0045cd70(char *obj, RwMatrix *from, RwMatrix *to);
extern int SUB_00484b20(int a, int b);
extern void SUB_00484960(int a, int b);
extern void SUB_0045db70(char *scene, float dt);
extern void SUB_0045d8c0(int a);

/* Clears the two 3-vectors (+0x0 and +0x10) of the physics body record that the rwp object `object` currently selects; nothing when it has none. */
static void warzone_body_clear_velocities(int object)
{
  int handle;
  int *body;
  RwV3d *v;
  RwV3d zero;

  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  handle = rwp_object_get_handle(object);
  if (handle == 0) {
    return;
  }
  handle = FUN_0058c8a0(handle);
  if (handle == 0) {
    return;
  }
  body = *(int **)(*(int *)(handle + 0x10) + 4);
  if (body == 0) {
    return;
  }
  v = (RwV3d *)(*(int *)(*(int *)(*body + 0x10) + 8) + body[1] * 0x20);
  *v = zero;
  v = (RwV3d *)(*(int *)(*(int *)(*body + 0x10) + 8) + 0x10 + body[1] * 0x20);
  *v = zero;
}

/* Per frame update of the warzone course: shake decay, the scene object `scene` and its children advance by dt = elapsed milliseconds / 3000;
   the two moving physics objects are placed from the course path state and their velocities cleared. */
void course_warzone_cb1(char *scene, unsigned int elapsed_ms)
{
  float dt;
  int i;

  dt = (float)elapsed_ms * 0.00033333333f;
  camera_shake_decay(dt);
  SUB_0045d720(g_course_warzone_8bf044, *(int *)(scene + 0xc010), dt);
  ptank_lock_all(get_viewport_camera(0));
  SUB_0045e0f0(scene);
  SUB_0045d1b0(g_course_6a2888, elapsed_ms);
  SUB_0045cd70(g_course_6a2888, &g_warzone_matrix_a, &g_warzone_matrix_a);
  rwp_object_set_matrix(g_course_60d9c8, &g_warzone_matrix_a);
  warzone_body_clear_velocities(g_course_60d9c8);
  SUB_0045cd70(g_course_6a2888, &g_warzone_matrix_b, &g_warzone_matrix_b);
  rwp_object_set_matrix(g_course_60d9cc, &g_warzone_matrix_b);
  warzone_body_clear_velocities(g_course_60d9cc);
  SUB_00484960(*(int *)(scene + 0xc110), SUB_00484b20(g_course_warzone_6a27a0, elapsed_ms * 2));
  SUB_0045db70(scene, dt);
  for (i = 0; i < 8; i++) {
    SUB_0045d8c0(g_course_warzone_8bf060[i]);
  }
}

