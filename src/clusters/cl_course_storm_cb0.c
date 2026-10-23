// cluster-entries: course_storm_cb0
/* Installed cluster course_storm_cb0 (tools/cluster_install.py from output/cluster_pilot_pg2/course_storm_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040d0d0*/ extern void *__cdecl find_texture_dict_b(const char *name);
/*00482270*/ extern float __cdecl spline_get_length(void *spline);
/*00481fd0*/ extern double __cdecl random_range(float low, float high);
/*0049a9b0*/ extern int __cdecl fx_slot_find_free(int kind, void *position, int a, int b);
/*004aac50*/ extern void __cdecl course_storm_cb0_sub_4aac50(void *cell, void *spline, float from, float to, float random_b, float random_a);
/*004aaca0*/ extern void __cdecl course_storm_cb0_sub_4aaca0(void *texture_dict);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *matrix, struct RwV3d *translation, enum RwOpCombineType combine);
/*008c5280*/ extern RwMatrix g_course_storm_8c5280;
/*008c52c0*/ extern int g_course_storm_waterfall_fx[];          /* the fx slots of the waterfall sources, g_course_storm_6a263c entries */
/*0060d920*/ extern struct RwV3d g_course_storm_matrix_offset;
/*006a263c*/ extern int g_course_storm_6a263c;                  /* number of waterfall fx slots */
/*006a2640*/ extern int g_course_storm_6a2640;                  /* number of storm cells */
/*008c4c20*/ extern unsigned char g_storm_cells[];              /* cell records, stride 0x3c */
/*005de23c*/ extern float k_f_0_06666667;

#define STORM_SPLINE_COUNT 9                 /* the course has 9 splines the storm cells are laid along */
#define STORM_CELL_SIZE 0x3c
#define STORM_MATRIX_FLAGS 0x20003
#define WATERFALL_SOURCE_STRIDE 0x40
#define FX_SLOT_KIND_WATERFALL 2
#define FX_SLOT_LIFETIME 0x14

/* offsets into the course world object */
#define WORLD_WATERFALL_SOURCES 0x1830       /* array of 0x40 byte source records */
#define WORLD_WATERFALL_SOURCE_COUNT 0xc008
#define WORLD_SPLINES 0xc0b0                 /* STORM_SPLINE_COUNT spline pointers */

/* course_storm_cb0: course set-up callback of the storm course: resets the storm matrix, creates one fx slot per waterfall source of
   the world, loads the "wfall" textures and lays storm cells along the 9 splines (about one per 15 units of spline length, each
   with a random pair of parameters) */
void course_storm_cb0(unsigned char *world)
{
  int source;
  int count;
  int i;
  float step;
  float random_a;
  float random_b;
  void **spline;
  int n;

  g_course_storm_8c5280.flags |= STORM_MATRIX_FLAGS;
  g_course_storm_8c5280.at.z = 1.0f;
  g_course_storm_8c5280.up.y = 1.0f;
  g_course_storm_8c5280.right.x = 1.0f;
  g_course_storm_8c5280.up.x = 0.0f;
  g_course_storm_8c5280.right.z = 0.0f;
  g_course_storm_8c5280.right.y = 0.0f;
  g_course_storm_8c5280.at.y = 0.0f;
  g_course_storm_8c5280.at.x = 0.0f;
  g_course_storm_8c5280.up.z = 0.0f;
  g_course_storm_8c5280.pos.z = 0.0f;
  g_course_storm_8c5280.pos.y = 0.0f;
  g_course_storm_8c5280.pos.x = 0.0f;
  RwMatrixTranslate(&g_course_storm_8c5280, &g_course_storm_matrix_offset, 0);
  g_course_storm_6a263c = 0;
  for (source = 0; source < *(int *)(world + WORLD_WATERFALL_SOURCE_COUNT); source++) {
    g_course_storm_waterfall_fx[g_course_storm_6a263c] =
        fx_slot_find_free(FX_SLOT_KIND_WATERFALL, world + WORLD_WATERFALL_SOURCES + source * WATERFALL_SOURCE_STRIDE, 0, FX_SLOT_LIFETIME);
    g_course_storm_6a263c++;
  }
  course_storm_cb0_sub_4aaca0(find_texture_dict_b("wfall"));
  g_course_storm_6a2640 = 0;
  spline = (void **)(world + WORLD_SPLINES);
  for (n = 0; n < STORM_SPLINE_COUNT; n++, spline++) {
    count = (int)(spline_get_length(*spline) * k_f_0_06666667 + 1.0f);
    step = 1.0f / (float)count;
    for (i = 0; i < count; i++) {
      random_a = (float)random_range(0.05f, 0.1f);
      random_b = (float)random_range(0.5f, 0.75f);
      course_storm_cb0_sub_4aac50(g_storm_cells + g_course_storm_6a2640 * STORM_CELL_SIZE, *spline, (float)i * step, (float)(i + 1) * step, random_b, random_a);
      g_course_storm_6a2640++;
    }
  }
}

