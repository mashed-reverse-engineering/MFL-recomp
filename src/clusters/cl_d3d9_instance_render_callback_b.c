// cluster-entries: d3d9_instance_render_callback_b
/* Installed cluster d3d9_instance_render_callback_b (tools/cluster_install.py from output/cluster_pilot_zy/d3d9_instance_render_callback_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004e3c00*/ extern void RwD3D9SetRenderState(unsigned int state, unsigned int value);
/*004e3c70*/ extern void RwD3D9SetTextureStageState(unsigned int stage, unsigned int type, unsigned int value);
/*004e3b30*/ extern void _rwD3D9RenderStateFlushCache(void);
/*004e55d0*/ extern void FUN_004e55d0(int texture, int stage);
/*007d2240*/ extern int **g_d3d9_device;
/*0062e768*/ extern int g_rp_62e768;
/*0062e76c*/ extern int g_rp_62e76c;

typedef int (__stdcall *D3DMethod1)(void *device, int a);
typedef int (__stdcall *D3DMethod6)(void *device, int a, int b, int c, int d, int e, int f);

/* RenderWare D3D9 instance render, second texture-stage variant (stage 0 alpha operand 1 instead of 2): sets the alpha-test state from bit 0x10 of `flags`, binds the instance's vertex shader (+0x10) and clears the pixel shader through the device,
   programs stage 0 / 1 for a plain textured draw with the texture of `texture_holder` (first word) and draws the indexed primitive (device vtable +0x148) from the instance record `instance` (esi). */
void d3d9_instance_render_callback_b_n(int *texture_holder, int *instance, int *mesh, unsigned int flags)
{
  int texture = *texture_holder;

  RwD3D9SetRenderState(0x89, (flags & 0x10) != 0);
  if (g_rp_62e768 != instance[4]) {
    g_rp_62e768 = instance[4];
    ((D3DMethod1)(*g_d3d9_device)[0x170 / 4])(g_d3d9_device, instance[4]);
  }
  if (g_rp_62e76c != 0) {
    g_rp_62e76c = 0;
    ((D3DMethod1)(*g_d3d9_device)[0x1ac / 4])(g_d3d9_device, 0);
  }
  FUN_004e55d0(texture, 0);
  RwD3D9SetTextureStageState(0, 1, 4);
  RwD3D9SetTextureStageState(0, 2, 2);
  RwD3D9SetTextureStageState(0, 3, 0);
  RwD3D9SetTextureStageState(0, 4, 1);
  RwD3D9SetTextureStageState(0, 5, 2);
  RwD3D9SetTextureStageState(0, 6, 0);
  FUN_004e55d0(0, 1);
  RwD3D9SetTextureStageState(1, 1, 1);
  RwD3D9SetTextureStageState(1, 4, 1);
  _rwD3D9RenderStateFlushCache();
  ((D3DMethod6)(*g_d3d9_device)[0x148 / 4])(g_d3d9_device, mesh[3], instance[5], 0, instance[6], instance[7], instance[8]);
}

/* entry thunk at the original address of d3d9_instance_render_callback_b: register inputs (eax,esi,s0,s1) -> natural C d3d9_instance_render_callback_b_n; ret 0 */
__declspec(naked) void d3d9_instance_render_callback_b(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push esi
        push eax
        call d3d9_instance_render_callback_b_n
        add esp, 16
        ret
    }
}
