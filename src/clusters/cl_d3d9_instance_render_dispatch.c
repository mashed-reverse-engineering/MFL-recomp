// cluster-entries: d3d9_instance_render_dispatch
/* Installed cluster d3d9_instance_render_dispatch (tools/cluster_install.py from output/cluster_pilot_uc3/d3d9_instance_render_dispatch.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
extern int FUN_004aa220(void);
extern int FUN_004aa250(float *v);
extern int FUN_004aa280(float *v);
extern int FUN_004aa2b0(void *instance, void *mesh, int a, int b, int c, int d);
extern int FUN_004aa530(void *instance, void *mesh, int a, int b, int c, int d);
extern int FUN_004aa740(void *instance, void *mesh, int a, int b, int c, int d);
extern int app_get_app_771058(void);
extern int _rwD3D9SetStreams(void *streams, int flag);
extern int RpMaterialSetTexture(void *material, int texture);
extern int g_app_62e770, g_rw_62e764, g_app_62e760, g_app_89de04;
extern int *g_d3d9_device;

/* Renders the meshes of an instanced geometry (`obj`, eax): sets the index buffer, the vertex streams and the vertex declaration when they changed, then per mesh
   chooses by the material's render type (1..4 in the plugin data of the material) and the pipeline mode (app_get_app_771058: 0 fixed function callbacks, 1 / 2 the
   shader paths) between the render callbacks. */
static int d3d9_instance_render_callback_a_r(void * p0, void * p1, void * p2, unsigned int p3);
extern void d3d9_instance_render_callback_a(void);
static int d3d9_instance_render_callback_b_r(void * p0, void * p1, void * p2, unsigned int p3);
extern void d3d9_instance_render_callback_b(void);
void d3d9_instance_render_dispatch_n(char *obj, int param_2, int param_3)
{
  char *instance;
  char *mesh;
  char *material;
  char *data;
  int index_buffer;
  int count;
  int mode;
  int a;
  int b;
  int plugin;
  float v[2];

  instance = obj + 0x18;
  index_buffer = *(int *)(obj + 0x20);
  if (g_app_62e770 != index_buffer) {
    g_app_62e770 = index_buffer;
    (*(int (__stdcall **)(int *, int))(*g_d3d9_device + 0x1a0))(g_d3d9_device, *(int *)(instance + 8));
  }
  _rwD3D9SetStreams(instance + 0x10, *(int *)(instance + 0x30));
  if (g_rw_62e764 != *(int *)(instance + 0x34)) {
    g_rw_62e764 = *(int *)(instance + 0x34);
    g_app_62e760 = -1;
    (*(int (__stdcall **)(int *, int))(*g_d3d9_device + 0x15c))(g_d3d9_device, *(int *)(instance + 0x34));
  }
  count = *(int *)(instance + 4);
  mesh = instance + 0x40;
  mode = app_get_app_771058();
  if (count == 0) {
    return;
  }
  plugin = g_app_89de04;
  do {
    material = *(char **)(mesh + 8);
    data = material + plugin;
    switch (*(int *)data) {
    case 1:
    case 2:
      if (mode == 0) {
        goto set_texture;
      }
      else if (mode == 1) {
        a = *(int *)(data + 4);
        b = *(int *)(data + 8);
        FUN_004aa220();
        FUN_004aa2b0(instance, mesh, param_3, a, b, param_2);
        plugin = g_app_89de04;
      }
      else if (mode == 2) {
        FUN_004aa530(instance, mesh, param_3, *(int *)(data + 4), *(int *)(data + 8), param_2);
        plugin = g_app_89de04;
      }
      break;
    case 3:
      if (mode == 0) {
        goto set_texture;
      }
      if (mode == 1) {
        a = *(int *)(data + 4);
        v[0] = -*(float *)(data + 0x20);
        v[1] = *(float *)(data + 0x24);
        b = *(int *)(data + 8);
        FUN_004aa280(v);
        FUN_004aa2b0(instance, mesh, param_3, a, b, param_2);
        plugin = g_app_89de04;
      }
      else if (mode == 2) {
        a = *(int *)(data + 4);
        v[0] = -*(float *)(data + 0x20);
        v[1] = *(float *)(data + 0x24);
        b = *(int *)(data + 8);
        FUN_004aa280(v);
        FUN_004aa740(instance, mesh, param_3, a, b, param_2);
        plugin = g_app_89de04;
      }
      break;
    case 4:
      if (mode == 0) {
        goto set_texture;
      }
      if (mode == 1) {
        a = *(int *)(data + 4);
        v[0] = -*(float *)(data + 0x20);
        v[1] = *(float *)(data + 0x24);
        b = *(int *)(data + 8);
        FUN_004aa250(v);
        FUN_004aa2b0(instance, mesh, param_3, a, b, param_2);
        plugin = g_app_89de04;
      }
      else if (mode == 2) {
set_texture:
        RpMaterialSetTexture(material, *(int *)(data + 4));
        d3d9_instance_render_callback_b_r(material, mesh, instance, param_2);
        plugin = g_app_89de04;
      }
      break;
    default:
      d3d9_instance_render_callback_a_r(material, mesh, instance, param_2);
      plugin = g_app_89de04;
    }
    mesh = mesh + 0x24;
    count = count - 1;
  } while (count != 0);
}

/* d3d9_instance_render_callback_a_r: C -> original d3d9_instance_render_callback_a (registers eax,esi,s0,s1) */
__declspec(naked) int d3d9_instance_render_callback_a_r(void * p0, void * p1, void * p2, unsigned int p3)
{
    __asm {
        push esi
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call d3d9_instance_render_callback_a
        add esp, 8
        pop esi
        ret
    }
}

/* d3d9_instance_render_callback_b_r: C -> original d3d9_instance_render_callback_b (registers eax,esi,s0,s1) */
__declspec(naked) int d3d9_instance_render_callback_b_r(void * p0, void * p1, void * p2, unsigned int p3)
{
    __asm {
        push esi
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov esi, dword ptr [esp + 20]
        call d3d9_instance_render_callback_b
        add esp, 8
        pop esi
        ret
    }
}

/* entry thunk at the original address of d3d9_instance_render_dispatch: register inputs (eax,s0,s1) -> natural C d3d9_instance_render_dispatch_n; ret 0 */
__declspec(naked) void d3d9_instance_render_dispatch(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call d3d9_instance_render_dispatch_n
        add esp, 12
        ret
    }
}
