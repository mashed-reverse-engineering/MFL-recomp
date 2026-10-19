#include "mfl_tier2.h"
/*004a2d20*/ extern int rw_set_value_at_root_plus_947144();
/*0064d8c0*/ extern int *g_app_64d8c0;
/*0064d8c4*/ extern undefined4 *g_log_64d8c4;
/*0064d8c8*/ extern undefined4 g_movie_64d8c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static void movie_draw_fullscreen_quad_r(int reg_eax, int reg_ecx, void *reg_ebx, int stk0, int stk1);
extern void movie_draw_fullscreen_quad(void);
/* adapter for the register convention callee 0x00403f80: eax / ecx / ebx = register inputs (0, 0, camera), stack = (0, movie) */
__declspec(naked) void movie_draw_fullscreen_quad_r(int reg_eax, int reg_ecx, void *reg_ebx, int stk0, int stk1)
{
    __asm {
        push ebx
        push dword ptr [esp + 0x18]
        push dword ptr [esp + 0x18]
        mov ebx, dword ptr [esp + 0x18]
        mov ecx, dword ptr [esp + 0x14]
        mov eax, dword ptr [esp + 0x10]
        call movie_draw_fullscreen_quad
        add esp, 8
        pop ebx
        ret
    }
}

/* DirectDraw surface COM methods (stdcall: the callee pops the arguments) */
typedef int (__stdcall *AppMethod48)(int *self,int a,int **out);
typedef int (__stdcall *LogMethod48)(undefined4 *self,int a,int b,int c,int **out);
typedef int (__stdcall *LogMethod88)(undefined4 *self,int *dst,int a,int *src,int b,int c);
typedef int (__stdcall *ReleaseMethod)(int *self);
#define SURFACE_VTBL_METHOD48 (0x48)
#define SURFACE_VTBL_METHOD88 (0x88)
#define COM_VTBL_RELEASE (0x8)
/* movie_present_frame */

/* "movie surface present/blit sequence (g_app_64d8c0/g_log_64d8c4)" */

void movie_present_frame(void)

{
  int *surface_a;
  int *surface_b;
  
  if (g_app_64d8c0 != (int *)0x0) {
    surface_a = (int *)0x0;
    surface_b = (int *)0x0;
    (*(AppMethod48 *)(*g_app_64d8c0 + SURFACE_VTBL_METHOD48))(g_app_64d8c0,0,&surface_a);
    (*(LogMethod48 *)(*g_log_64d8c4 + SURFACE_VTBL_METHOD48))(g_log_64d8c4,0,0,0,&surface_b);
    (*(LogMethod88 *)(*g_log_64d8c4 + SURFACE_VTBL_METHOD88))(g_log_64d8c4,surface_b,0,surface_a,0,0);
    rw_set_value_at_root_plus_947144(g_movie_64d8c8,g_app_64d8c0);
    movie_draw_fullscreen_quad_r(0,0,RwCameraGetCurrentCamera(),0,g_movie_64d8c8);
    (*(ReleaseMethod *)(*surface_a + COM_VTBL_RELEASE))(surface_a);
    (*(ReleaseMethod *)(*surface_b + COM_VTBL_RELEASE))(surface_b);
  }
  return;
}
