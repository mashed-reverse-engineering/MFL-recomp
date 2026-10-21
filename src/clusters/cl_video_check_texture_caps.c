// cluster-entries: video_check_texture_caps
/* Installed cluster video_check_texture_caps (tools/cluster_install.py from output/cluster_pilot_uc3/video_check_texture_caps.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a5530*/ extern void __cdecl log_print(const char *text);
/*004a5550*/ extern int log_printf(const char *format, ...);
/*004a8830*/ extern int app_get_app_62c5c8(void);
/*004a8840*/ extern int app_get_app_62c5cc(void);
/*004a8850*/ extern int app_get_app_771504(void);
/*004a8860*/ extern int app_get_app_7712f4(void);
/*004d7950*/ extern struct RwRaster *RwRasterCreate(int width, int height, int depth, int flags);

/* A COM object: first member is the pointer to the method table. */
typedef struct ComObject { void **vtbl; } ComObject;
extern ComObject *g_d3dx_76e898;      /* the Direct3D9 device */
extern ComObject *g_log_64d8c4;       /* copy of the device pointer */
extern int g_log_64d898;              /* 0 once the back buffer texture could be created */
extern ComObject *g_app_64d8c0;       /* the created texture */

#define D3DPTEXTURECAPS_NONPOW2CONDITIONAL 0x100
#define D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES 0x10
#define D3DFMT_A8R8G8B8 0x15
#define D3DFMT_X8R8G8B8 0x16
#define D3DFMT_R5G6B5 0x17

/* Device methods used (IDirect3DDevice9 vtable index): GetDeviceCaps 7, CreateTexture 23, GetRenderTarget 38; surface: Release 2, GetDesc 12. */
#define DEVICE_GET_DEVICE_CAPS(dev, caps)  ((int (__stdcall *)(ComObject *, unsigned int *))(dev)->vtbl[7])(dev, caps)
#define DEVICE_CREATE_TEXTURE(dev, w, h, levels, usage, fmt, pool, out, shared) ((int (__stdcall *)(ComObject *, int, int, int, int, int, int, ComObject **, void *))(dev)->vtbl[23])(dev, w, h, levels, usage, fmt, pool, out, shared)
#define DEVICE_GET_RENDER_TARGET(dev, index, out) ((int (__stdcall *)(ComObject *, int, ComObject **))(dev)->vtbl[38])(dev, index, out)

/* Probes whether the card can create a texture of the back buffer's size and format (non power of two textures and StretchRect from textures are
   both needed): logs the findings, creates the texture into g_app_64d8c0 and clears g_log_64d898 on success (set to 1 otherwise).
   Always ends with the creation of a 0x84 flag raster. */
void video_check_texture_caps(void)
{
  unsigned int caps[76];          /* D3DCAPS9: TextureCaps at [15], DevCaps2 at [53] */
  ComObject *surface = 0;
  unsigned int desc[8];           /* D3DSURFACE_DESC: Format first */
  int width, height, format;
  int depth;

  g_log_64d898 = 1;
  g_log_64d8c4 = g_d3dx_76e898;
  if (DEVICE_GET_DEVICE_CAPS(g_d3dx_76e898, caps) >= 0) {
    int nonpow2 = caps[15] & D3DPTEXTURECAPS_NONPOW2CONDITIONAL;
    int stretch = caps[53] & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;

    if (nonpow2 != 0) {
      log_print("Video card supports D3DPTEXTURECAPS_NONPOW2CONDITIONAL\n");
    } else {
      log_print("Video card does NOT support D3DPTEXTURECAPS_NONPOW2CONDITIONAL\n");
    }
    if (stretch != 0) {
      log_print("Video card supports D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES\n");
    } else {
      log_print("Video card does NOT support D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES\n");
    }
    if (nonpow2 != 0 && stretch != 0) {
      width = app_get_app_62c5c8();
      height = app_get_app_62c5cc();
      DEVICE_GET_RENDER_TARGET(g_log_64d8c4, 0, &surface);
      ((int (__stdcall *)(ComObject *, unsigned int *))surface->vtbl[12])(surface, desc);
      ((int (__stdcall *)(ComObject *))surface->vtbl[2])(surface);
      if (app_get_app_7712f4() == 0) {
        /* the original falls back to the stale surface pointer value as the format when the target format is neither 21..23 (a bug of the game) */
        format = (int)surface;
        if ((int)desc[0] >= D3DFMT_A8R8G8B8) {
          if ((int)desc[0] <= D3DFMT_X8R8G8B8) {
            format = D3DFMT_X8R8G8B8;
          } else if (desc[0] == D3DFMT_R5G6B5) {
            format = D3DFMT_R5G6B5;
          }
        }
      } else {
        format = (int)surface;
        depth = app_get_app_771504();
        if (depth == 0x20) {
          format = D3DFMT_X8R8G8B8;
          log_print("format = D3DFMT_X8R8G8B8\n");
        } else if (depth == 0x10) {
          format = D3DFMT_R5G6B5;
          log_print("format = D3DFMT_R5G6B5\n");
        }
      }
      log_printf("Attempting to create texture for back buffer (%dx%d)\n", width, height);
      if (DEVICE_CREATE_TEXTURE(g_log_64d8c4, width, height, 1, 1, format, 0, &g_app_64d8c0, 0) < 0) {
        log_print("\tUnable to create texture\n");
      } else {
        log_print("\tCreated texture\n");
      }
      g_log_64d898 = 0;
    } else {
      g_log_64d898 = 1;
    }
  }
  RwRasterCreate(0, 0, 0, 0x84);
}

