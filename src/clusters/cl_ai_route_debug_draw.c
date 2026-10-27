// cluster-entries: ai_route_debug_draw
/* Installed cluster ai_route_debug_draw (tools/cluster_install.py from output/cluster_pilot_md/ai_route_debug_draw.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0044d2a0*/ extern void ai_nearest_route_point(float *route, float *position, float *nearest, int car, int use_hint, int draw);
/*00475730*/ extern int get_viewport_camera(int view);
/*0047b520*/ extern int car_current_matrix(RwMatrix **out, int car);
/*004c56e0*/ extern void FUN_004c56e0(int camera, float *from, float *to, unsigned char *colour);
/*00568cc0*/ extern int RtCharsetPrintBuffered(void *charset, char *text, int x, int y, int hide_spaces);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd378*/ extern float k_f_0_1;
/*005dd41c*/ extern float float_5_0;
/*005dd634*/ extern float k_f_2;
/*005de88c*/ extern float k_f_512;
/*005de1ac*/ extern float k_f_0_125;
/*005df304*/ extern float k_f_n10000;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eeb64*/ extern void *g_ai_7eeb64;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
/* ai_route_debug_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws the route polygon and Catmull-Rom spline of an AI route with point indices (debug)" */

/* Route point i is route[2 * i] (x), route[2 * i + 1] (z); the count is the int at byte offset 0x200. */
#define ROUTE_COUNT(route) (*(int *)((char *)(route) + 0x200))

/* screen unit: a world position is shown in a 16 x 16 box, 8 world units per unit */
static int screen_in_range(float a, float b)
{
  return k_f_n1 < a && k_f_n1 < b && a < k_f_2 && b < k_f_2;
}

void ai_route_debug_draw(float *route)
{
  struct {
    float p0[2];             /* previous / start point in screen units; first the car position */
    float t;
    float p1[2];             /* current point in screen units; first the nearest route point */
    unsigned char colour[4];
    int idx_d;
    int k;
    float origin_x;
    float origin_z;
    float *d_y;
    int pad_30;
    float x;
  } f;
  RwMatrix *car_matrix;
  char text[268];
  float *a, *b, *c;
  int count, i, j, idx;
  float oz, x, y;

  f.colour[0] = 0xff;
  f.colour[1] = 0;
  f.colour[2] = 0xff;
  f.colour[3] = 0xff;
  car_current_matrix(&car_matrix, 0);
  f.p0[0] = car_matrix->pos.x;
  f.p0[1] = car_matrix->pos.z;
  ai_nearest_route_point(route, f.p0, f.p1, 0, 1, 1);
  f.origin_x = g_ai_editor_view_origin_x + k_f_4;
  count = ROUTE_COUNT(route);
  f.origin_z = g_ai_editor_view_origin_z - k_f_4;
  /* the route polygon with the index of every point */
  for (i = 0; i < count; i = i + 1) {
    j = i + 1;
    if (j == count) {
      j = 0;
    }
    f.p0[0] = (route[i * 2 + 1] - f.origin_z) * k_f_0_125;
    f.p0[1] = (f.origin_x - route[i * 2]) * k_f_0_125;
    f.p1[0] = (route[j * 2 + 1] - f.origin_z) * k_f_0_125;
    f.p1[1] = (f.origin_x - route[j * 2]) * k_f_0_125;
    if (screen_in_range(f.p0[0], f.p1[0]) && screen_in_range(f.p0[1], f.p1[1])) {
      FUN_004c56e0(get_viewport_camera(0), f.p0, f.p1, f.colour);
    }
    rwsprintf(text, "%d", i);
    RtCharsetPrintBuffered(g_ai_7eeb64, text, (int)(f.p0[0] * k_f_512), (int)(f.p0[1] * k_f_512), 0);
    count = ROUTE_COUNT(route);
  }
  count = ROUTE_COUNT(route);
  if (count > 3) {
    /* the Catmull-Rom spline through the route, 11 samples per segment */
    f.colour[1] = 0xff;
    f.p0[0] = -10000.0f;
    f.p0[1] = -10000.0f;
    if (count > 0) {
      f.k = 1;
      a = route;
      do {
        idx = f.k;
        if (idx >= count) {
          idx = idx - count;
        }
        b = route + idx * 2;
        idx = idx + 1;
        if (idx >= count) {
          idx = idx - count;
        }
        c = route + idx * 2;
        idx = idx + 1;
        if (idx >= count) {
          idx = idx - count;
        }
        f.idx_d = idx;
        f.d_y = route + idx * 2 + 1;
        f.t = 0.0f;
        do {
          float t2 = f.t * f.t;
          float t3 = f.t * t2;
          f.x = (((((a[0] + a[0]) - b[0] * float_5_0) + c[0] * k_f_4) - route[idx * 2]) * t2 +
                 (((b[0] * k_f_3 - a[0]) - c[0] * k_f_3) + route[idx * 2]) * t3 +
                 (c[0] - a[0]) * f.t + (b[0] + b[0])) * k_f_0_5;
          y = (((((a[1] + a[1]) - b[1] * float_5_0) + c[1] * k_f_4) - *f.d_y) * t2 +
               (((b[1] * k_f_3 - a[1]) - c[1] * k_f_3) + *f.d_y) * t3 +
               (c[1] - a[1]) * f.t + (b[1] + b[1])) * k_f_0_5;
          f.p1[0] = (y - f.origin_z) * k_f_0_125;
          f.p1[1] = (f.origin_x - f.x) * k_f_0_125;
          if (f.p0[0] != k_f_n10000 && screen_in_range(f.p0[0], f.p1[0]) && screen_in_range(f.p0[1], f.p1[1])) {
            FUN_004c56e0(get_viewport_camera(0), f.p0, f.p1, f.colour);
          }
          f.p0[0] = f.p1[0];
          f.p0[1] = f.p1[1];
          f.t = f.t + k_f_0_1;
        } while (f.t <= float_1_0);
        count = ROUTE_COUNT(route);
        a = a + 2;
        f.k = f.k + 1;
      } while (f.k - 1 < count);
    }
  }
}

