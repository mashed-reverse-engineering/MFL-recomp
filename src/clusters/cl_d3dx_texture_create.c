// cluster-entries: d3dx_texture_create
/* Installed cluster d3dx_texture_create (tools/cluster_install.py from output/cluster_pilot_uc3/d3dx_texture_create.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a3420*/ extern void app_error_message_box(const char *format, ...);

/* A COM object: first member is the pointer to the method table. */
typedef struct ComObject { void **vtbl; } ComObject;
extern ComObject *g_d3dx_76e898;      /* the Direct3D9 device */
extern ComObject *g_d3dx_76e89c;      /* first (system memory) texture */
extern ComObject *g_app_76e8a0;       /* second texture */
extern int g_d3dx_texture_76e888;     /* pixel format of the texture */
extern float g_app_76e878, g_app_76e87c, g_app_76e880, g_app_76e884;     /* source size and texture size as floats */

#define D3DPTEXTURECAPS_POW2 2
/* Device: GetDeviceCaps 7, CreateTexture 23.  Texture: GetLevelDesc 17, GetSurfaceLevel 18.  Surface: Release 2, GetDesc 12. */
#define COM_METHOD(obj, index, type) ((type)(((ComObject *)(obj))->vtbl[index]))

/* A texture loader object (only the fields used here). */
typedef struct D3dxTexture {
  char pad_0[0x164];
  int width;                 /* +0x164 */
  int height;                /* +0x168 absolute value */
  int stride;                /* +0x16c bytes per row of 24 bit pixels, 4 byte aligned */
} D3dxTexture;
/* The source bitmap info the width / height come from (at +0x44 of the source). */
typedef struct BitmapInfo { char pad_0[0x34]; int width; int height; } BitmapInfo;

/* Creates the two textures for a bitmap `source` (the loader's own size fields are set from its header): the texture size is the bitmap size, or rounded
   up to powers of two (at least 2) when the card requires it (the unrounded size and the texture size are published as floats), the first texture
   becomes g_d3dx_76e89c, the second g_app_76e8a0; both are X8R8G8B8 (0x16), managed pool.  Then the format of the first texture's top level is read
   back and must be 0x16 or 0x19.  Returns 0, the failing HRESULT of the texture creation, or 0x8004022a for an unsupported format
   (the original also registers a C++ exception frame around the body; not reproduced). */
int d3dx_texture_create_n(D3dxTexture *this_, char *source)
{
  BitmapInfo *info;
  unsigned int caps[76];       /* D3DCAPS9: TextureCaps at [15] */
  unsigned int level_desc[8];  /* D3DSURFACE_DESC */
  ComObject *surface;
  int w2, h2;
  int hr;
  int format;
  int i;

  info = *(BitmapInfo **)(source + 0x44);
  this_->width = info->width;
  this_->height = (info->height ^ (info->height >> 31)) - (info->height >> 31);
  this_->stride = ((info->width + 1) * 3) & 0xfffffffc;
  for (i = 0; i < 76; i++) {
    caps[i] = 0;
  }
  COM_METHOD(g_d3dx_76e898, 7, int (__stdcall *)(ComObject *, unsigned int *))(g_d3dx_76e898, caps);
  w2 = 2;
  h2 = 2;
  if ((caps[15] & D3DPTEXTURECAPS_POW2) == 0) {
    w2 = this_->width;
    h2 = this_->height;
  } else {
    if (this_->width > 2) {
      do {
        w2 = w2 * 2;
      } while (w2 < this_->width);
    }
    if (this_->height > 2) {
      do {
        h2 = h2 * 2;
      } while (h2 < this_->height);
    }
  }
  g_app_76e878 = (float)this_->width;
  g_app_76e87c = (float)this_->height;
  g_app_76e880 = (float)(unsigned int)w2;
  g_app_76e884 = (float)(unsigned int)h2;
  hr = COM_METHOD(g_d3dx_76e898, 23, int (__stdcall *)(ComObject *, int, int, int, int, int, int, ComObject **, void *))(g_d3dx_76e898, w2, h2, 1, 0, 0x16, 1, &g_d3dx_76e89c, 0);
  if (hr >= 0) {
    hr = COM_METHOD(g_d3dx_76e898, 23, int (__stdcall *)(ComObject *, int, int, int, int, int, int, ComObject **, void *))(g_d3dx_76e898, w2, h2, 1, 0, 0x16, 1, &g_app_76e8a0, 0);
    if (hr >= 0) {
      for (i = 0; i < 8; i++) {
        level_desc[i] = 0;
      }
      COM_METHOD(g_d3dx_76e89c, 17, int (__stdcall *)(ComObject *, int, unsigned int *))(g_d3dx_76e89c, 0, level_desc);
      surface = 0;
      hr = COM_METHOD(g_d3dx_76e89c, 18, int (__stdcall *)(ComObject *, int, ComObject **))(g_d3dx_76e89c, 0, &surface);
      if (hr >= 0) {
        COM_METHOD(surface, 12, int (__stdcall *)(ComObject *, unsigned int *))(surface, level_desc);
      }
      format = level_desc[0];
      g_d3dx_texture_76e888 = format;
      if (format != 0x16 && format != 0x19) {
        app_error_message_box("Texture is format we can't handle! Format = 0x%x", format);
        if (surface != 0) {
          COM_METHOD(surface, 2, int (__stdcall *)(ComObject *))(surface);
        }
        return 0x8004022a;
      }
      if (surface != 0) {
        COM_METHOD(surface, 2, int (__stdcall *)(ComObject *))(surface);
      }
      return 0;
    }
  }
  app_error_message_box("Could not create the D3DX texture!  hr=0x%x", hr);
  return hr;
}

/* entry thunk at the original address of d3dx_texture_create: register inputs (ecx,s0) -> natural C d3dx_texture_create_n; ret 4 */
__declspec(naked) void d3dx_texture_create(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call d3dx_texture_create_n
        add esp, 8
        ret 4
    }
}
