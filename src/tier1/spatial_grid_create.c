#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00474910*/ extern void spatial_grid_clear(void);
/*005dd330*/ extern float float_1_0;
/*006c2884*/ extern void *g_car_6c2884;

/*006c28a0*/ extern float g_car_6c28a0;
/*006c28a4*/ extern float g_car_6c28a4;
/*006c28a8*/ extern float g_car_6c28a8;
/*006c28ac*/ extern float g_car_6c28ac;
/*006c28b0*/ extern int g_car_6c28b0;
/*006c28b4*/ extern int g_car_6c28b4;
/*006c28b8*/ extern float g_car_6c28b8;
/*006c28bc*/ extern float g_car_6c28bc;
struct GridBounds { float v[6]; };
/*006c2888*/ extern struct GridBounds g_car_6c2888;
extern void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
/* spatial_grid_create */
/* "(w, h): clears the header and allocates w * h dwords" */
void __cdecl spatial_grid_create(int grid_w, int grid_h, struct GridBounds *bounds)
{
  int size;

  spatial_grid_clear();
  size = grid_w * grid_h * 4;
  g_car_6c2884 = RwMalloc(size, 0);
  memset(g_car_6c2884, 0, size);
  g_car_6c28b0 = grid_h;
  g_car_6c28b4 = grid_w;
  g_car_6c2888 = *bounds;
  g_car_6c28a0 = bounds->v[0] - bounds->v[3];
  g_car_6c28a4 = bounds->v[2] - bounds->v[5];
  g_car_6c28a8 = float_1_0 / g_car_6c28a0;
  g_car_6c28ac = float_1_0 / g_car_6c28a4;
  g_car_6c28bc = g_car_6c28a0 / (float)grid_w;
  g_car_6c28b8 = g_car_6c28a4 / (float)grid_h;
}
