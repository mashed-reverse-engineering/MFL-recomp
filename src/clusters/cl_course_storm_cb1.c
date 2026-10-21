// cluster-entries: course_storm_cb1
/* Installed cluster course_storm_cb1 (tools/cluster_install.py from output/cluster_pilot_uc3/course_storm_cb1.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern int g_course_storm_6a2640;
extern int g_course_storm_8c53c0;
extern char g_course_storm_8c5280[];
extern float g_gravity_vector;
extern float g_cars_801b20;
extern float g_cars_801b24;
extern float g_cars_801b28;
/*00481fd0*/ extern double random_range(float low, float high);
/*00475730*/ extern RwCamera *get_viewport_camera(int index);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(RwCamera *camera, RwSphere *sphere);
extern void SUB_004ab160(void *cell, float dt);
extern void SUB_00471eb0(RwSphere *sphere);
extern void SUB_004aac90(void *cell, float dt);
extern void SUB_004aaf40(void);
extern void SUB_00485250(void *obj, RwV3d *axis, float angle);

/* One lightning cell of 60 bytes: 32 bytes of state, its bounding sphere, 12 bytes of tail. */
typedef struct StormCell { char state[0x20]; RwSphere sphere; char tail[0xc]; } StormCell;
extern StormCell g_storm_cells[];

/* Per frame update of the storm course: a random cell is advanced once more when its sphere is inside the view frustum, then all cells are
   advanced by dt = elapsed milliseconds / 3000, gravity is reset to (-1, 0, 0) with strength 5 and the storm object is turned. */
void course_storm_cb1(int unused, unsigned int elapsed_ms)
{
  RwV3d axis;
  int i;
  int random_cell;

  random_cell = (int)random_range(0.0f, (float)g_course_storm_6a2640);
  if (RwCameraFrustumTestSphere(get_viewport_camera(0), &g_storm_cells[random_cell].sphere) != rwSPHEREOUTSIDE) {
    SUB_004ab160(&g_storm_cells[random_cell], (float)elapsed_ms * 0.00033333333f);
    SUB_00471eb0(&g_storm_cells[random_cell].sphere);
  }
  for (i = 0; i < g_course_storm_6a2640; i++) {
    SUB_004aac90(&g_storm_cells[i], (float)elapsed_ms * 0.00033333333f);
  }
  SUB_004aaf40();
  g_gravity_vector = -1.0f;
  g_cars_801b20 = 0.0f;
  g_cars_801b24 = 0.0f;
  g_cars_801b28 = 5.0f;
  axis.x = 0.0f;
  axis.y = 1.0f;
  axis.z = 0.0f;
  SUB_00485250(g_course_storm_8c5280, &axis, (float)g_course_storm_8c53c0 * 0.01f);
}

