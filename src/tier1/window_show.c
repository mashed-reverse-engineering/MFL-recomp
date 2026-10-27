#include "mfl_tier2.h"
extern __declspec(dllimport) int __stdcall ShowWindow(int, int);
extern __declspec(dllimport) int __stdcall UpdateWindow(int);
/*007e7704*/ extern undefined4 g_app_7e7704;


/* window_show */

/* "ShowWindow + UpdateWindow(g_app_7e7704)" */

void window_show(int show_cmd)
{
  ShowWindow(g_app_7e7704,show_cmd);
  UpdateWindow(g_app_7e7704);
  return;
}
