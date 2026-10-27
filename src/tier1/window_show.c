#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall ShowWindow(int, int);
extern __declspec(dllimport) int __stdcall UpdateWindow(int);
/*007e7704*/ extern undefined4 g_app_7e7704;


/* window_show */

/* "ShowWindow + UpdateWindow(g_app_7e7704)" */

void window_show(int param_1)
{
  ShowWindow(g_app_7e7704,param_1);
  UpdateWindow(g_app_7e7704);
  return;
}
