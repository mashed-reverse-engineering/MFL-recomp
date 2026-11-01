// cluster-entries: video_present_frame
/* Installed cluster video_present_frame (tools/cluster_install.py from output/cluster_pilot_b6/video_present_frame.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0064d8c0*/ extern int **g_app_64d8c0;
/*00771034*/ extern int **g_app_771034;
/*00771038*/ extern void *g_app_771038;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*004a2d20*/ extern void __cdecl rw_set_value_at_root_plus_947144(void *p0, void *p1);

/* byte offsets into the COM vtables of the two Direct3D objects (g_app_64d8c0 / g_app_771034) and of the surfaces */
#define COM_METHOD(obj, off) ((*(int **)(obj))[(off) / 4])
#define VTBL_GET_SURFACE 0x48
#define VTBL_COPY_SURFACE 0x88
#define VTBL_RELEASE 8
typedef int (__stdcall *ComGetSurface3)(void *self, int a, void *out);
typedef int (__stdcall *ComGetSurface5)(void *self, int a, int b, int c, void *out);
typedef int (__stdcall *ComCopy6)(void *self, void *src, int a, void *dst, int b, int c);
typedef int (__stdcall *ComRelease)(void *self);

/* video_present_frame(unused, alpha): when the output surface object exists, draws the full quad of the current texture in white with the given alpha, grabs the two
   surfaces of the presenting objects, copies one to the other, updates the RenderWare root value and releases both surfaces. */
static void video_draw_textured_quad_b_r(const float * p0, const unsigned char * p1, void * p2, const float * p3, void * p4);
extern void video_draw_textured_quad_b(void);
void video_present_frame(int unused, int alpha)
{
  void *surface_a = 0;
  void *surface_b = 0;
  unsigned char colour[4];

  if (g_app_64d8c0 != 0) {
    colour[0] = 0xff;
    colour[1] = 0xff;
    colour[2] = 0xff;
    colour[3] = (unsigned char)alpha;
    video_draw_textured_quad_b_r(0, colour, *(void **)RwEngineInstance, 0, g_app_771038);
    ((ComGetSurface3)COM_METHOD(g_app_64d8c0, VTBL_GET_SURFACE))(g_app_64d8c0, 0, &surface_a);
    ((ComGetSurface5)COM_METHOD(g_app_771034, VTBL_GET_SURFACE))(g_app_771034, 0, 0, 0, &surface_b);
    ((ComCopy6)COM_METHOD(g_app_771034, VTBL_COPY_SURFACE))(g_app_771034, surface_b, 0, surface_a, 0, 0);
    rw_set_value_at_root_plus_947144(g_app_771038, g_app_64d8c0);
    ((ComRelease)COM_METHOD(surface_a, VTBL_RELEASE))(surface_a);
    ((ComRelease)COM_METHOD(surface_b, VTBL_RELEASE))(surface_b);
  }
}

/* video_draw_textured_quad_b_r: C -> original video_draw_textured_quad_b (registers ecx,eax,ebx,s0,s1) */
__declspec(naked) void video_draw_textured_quad_b_r(const float * p0, const unsigned char * p1, void * p2, const float * p3, void * p4)
{
    __asm {
        push ebx
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        mov ecx, dword ptr [esp + 16]
        mov eax, dword ptr [esp + 20]
        mov ebx, dword ptr [esp + 24]
        call video_draw_textured_quad_b
        add esp, 8
        pop ebx
        ret
    }
}
