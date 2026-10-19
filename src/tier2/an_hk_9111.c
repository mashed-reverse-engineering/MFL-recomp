#include "mfl_tier2.h"
/*0076e89c*/ extern undefined4 *g_d3dx_76e89c;
/*0076e8a0*/ extern undefined4 *g_app_76e8a0;
/*0076e8ac*/ extern int *g_app_76e8ac;
/*0076e8b0*/ extern int *g_movie_renderer_76e8b0;
/*0076e8b4*/ extern int *g_movie_renderer_76e8b4;
/*0076e8b8*/ extern int *g_movie_renderer_76e8b8;
/*0076e8bc*/ extern int *g_movie_renderer_76e8bc;
/*0076e8c0*/ extern int *g_movie_renderer_76e8c0;
/* movie_renderer_release */

/* "releases the three COM interfaces of the movie renderer" */

void movie_renderer_release(void)

{
  int *piVar1;
  
  if (g_movie_renderer_76e8c0 != (int *)0x0) {
    (*(int (__stdcall **)(int *))(*g_movie_renderer_76e8c0 + 0x24))(g_movie_renderer_76e8c0);
    piVar1 = g_movie_renderer_76e8c0;
    if (g_movie_renderer_76e8c0 != (int *)0x0) {
      g_movie_renderer_76e8c0 = (int *)0x0;
      (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
    }
  }
  piVar1 = g_movie_renderer_76e8b4;
  if (g_movie_renderer_76e8b4 != (int *)0x0) {
    g_movie_renderer_76e8b4 = (int *)0x0;
    (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
  }
  piVar1 = g_movie_renderer_76e8b8;
  if (g_movie_renderer_76e8b8 != (int *)0x0) {
    g_movie_renderer_76e8b8 = (int *)0x0;
    (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
  }
  piVar1 = g_movie_renderer_76e8bc;
  if (g_movie_renderer_76e8bc != (int *)0x0) {
    g_movie_renderer_76e8bc = (int *)0x0;
    (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
  }
  piVar1 = g_movie_renderer_76e8b0;
  if (g_movie_renderer_76e8b0 != (int *)0x0) {
    g_movie_renderer_76e8b0 = (int *)0x0;
    (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
  }
  piVar1 = g_app_76e8ac;
  if (g_app_76e8ac != (int *)0x0) {
    g_app_76e8ac = (int *)0x0;
    (*(int (__stdcall **)(int *))(*(int *)piVar1 + 8))((int *)piVar1);
  }
  (*(int (__stdcall **)(int *))(*(int *)g_d3dx_76e89c + 8))((int *)g_d3dx_76e89c);
  (*(int (__stdcall **)(int *))(*(int *)g_app_76e8a0 + 8))((int *)g_app_76e8a0);
  return;
}


