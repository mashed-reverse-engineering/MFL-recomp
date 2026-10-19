// cluster-entries: movie_renderer_construct
/* Installed cluster movie_renderer_construct (tools/cluster_install.py from output/cluster_pilot_hb1/movie_renderer_construct.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char PTR_LAB_005e0ec0[];
extern char PTR_FUN_005e0e80[];
extern char PTR_LAB_005e238c[];
extern char PTR_LAB_005e2368[];
extern char PTR_LAB_005e2354[];
extern char g_movie_renderer_5e0f84[];
/* movie_renderer_construct */

static int * FUN_004aeb20_r(int * p0, void * p1, int p2, int p3, int * p4);
extern void FUN_004aeb20(void);
int *movie_renderer_construct_n(int *in_ecx, int param_1, int *param_2)

{
  FUN_004aeb20_r(in_ecx,g_movie_renderer_5e0f84,0,param_1,param_2);
  *in_ecx = (int)PTR_LAB_005e0ec0;
  in_ecx[3] = (int)PTR_FUN_005e0e80;
  in_ecx[4] = (int)PTR_LAB_005e238c;
  in_ecx[0x38] = (int)PTR_LAB_005e2368;
  in_ecx[0x39] = (int)PTR_LAB_005e2354;
  in_ecx[0x58] = 0;
  if (param_2 != (int *)0x0) {
    *param_2 = 0;
  }
  return in_ecx;
}

/* FUN_004aeb20_r: C -> original FUN_004aeb20 (registers ecx,s0,s1,s2,s3) */
__declspec(naked) int * FUN_004aeb20_r(int * p0, void * p1, int p2, int p3, int * p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov ecx, dword ptr [esp + 20]
        call FUN_004aeb20
        ret
    }
}

/* entry thunk at the original address of movie_renderer_construct: register inputs (ecx,s0,s1) -> natural C movie_renderer_construct_n; ret 8 */
__declspec(naked) void movie_renderer_construct(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push ecx
        call movie_renderer_construct_n
        add esp, 12
        ret 8
    }
}
