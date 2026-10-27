// cluster-entries: movie_fn_4a31b0
/* Installed cluster movie_fn_4a31b0 (tools/cluster_install.py from output/cluster_pilot_kg/movie_fn_4a31b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0076e888*/ extern int g_d3dx_texture_76e888;
/*0076e88c*/ extern int g_movie_renderer_76e88c;
/*0076e89c*/ extern void * g_d3dx_76e89c[2];
/*0062ad50*/ extern int g_app_62ad50;
/* movie_fn_4a31b0 */

#define D3DFMT_X8R8G8B8 0x16
#define D3DFMT_A1R5G5B5 0x19
#define E_FAIL_HRESULT 0x80004005
#define VTABLE_FRAME_SOURCE_GET_DATA 0xc  /* the video frame source: (source, &data) hands out the pointer to the decoded RGB24 frame */
#define VTABLE_TEXTURE_LOCK_RECT 0x4c     /* IDirect3DTexture9::LockRect(level, locked, rect, flags) */
#define VTABLE_TEXTURE_UNLOCK_RECT 0x50

typedef struct MovieFrameInfo {
  char _gap_0[0x164];
  int width;        /* +0x164 pixels per row */
  unsigned height;  /* +0x168 rows */
  int source_pitch; /* +0x16c bytes per row of the RGB24 source frame */
} MovieFrameInfo;

typedef struct LockedRect {
  int pitch;
  void *bits;
} LockedRect;

/* "copies the decoded RGB24 video frame of the movie into the streaming texture (X8R8G8B8 or A1R5G5B5) that is currently not displayed, then flips the two textures"
   movie = the frame info (ecx), source = the video frame source object (stack argument). Returns 0 (also when no texture exists) or E_FAIL */
int movie_fn_4a31b0_n(MovieFrameInfo *movie, void **source)

{
  void *texture;
  void *frame;
  unsigned char *src;
  unsigned char *row_src;
  unsigned char *dst;
  unsigned char *row_dst;
  unsigned *out;
  unsigned *in;
  unsigned remainder;
  unsigned count;
  unsigned y;
  int n4;
  int x;
  int swap_index;
  LockedRect locked;

  texture = g_d3dx_76e89c[g_movie_renderer_76e88c];
  if (texture == 0) {
    return 0;
  }
  frame = source;
  (*(int (__stdcall **)(void *, void **))((char *)*(void **)source + VTABLE_FRAME_SOURCE_GET_DATA))(source, &frame);
  if ((*(int (__stdcall **)(void *, int, LockedRect *, void *, int))((char *)*(void **)g_d3dx_76e89c[g_movie_renderer_76e88c] + VTABLE_TEXTURE_LOCK_RECT))(g_d3dx_76e89c[g_movie_renderer_76e88c], 0, &locked, 0, 0) < 0) {
    return E_FAIL_HRESULT;
  }
  src = (unsigned char *)frame;
  dst = (unsigned char *)locked.bits;
  if (g_d3dx_texture_76e888 == D3DFMT_X8R8G8B8) {
    n4 = (movie->width + ((movie->width >> 31) & 3)) >> 2;
    y = 0;
    if (movie->height != 0) {
      do {
        row_src = src;
        out = (unsigned *)dst;
        in = (unsigned *)src;
        for (count = n4; count != 0; count--) {
          out[0] = in[0] | 0xff000000;
          out[1] = ((in[1] | 0xffff0000) << 8) | (unsigned)*((unsigned char *)in + 3);
          out[2] = ((in[2] | 0xffffff00) << 16) | (unsigned)*(unsigned short *)((unsigned char *)in + 6);
          out[3] = (in[2] >> 8) | 0xff000000;
          in = in + 3;
          out = out + 4;
        }
        remainder = 0;
        if ((movie->width & 3) != 0) {
          src = (unsigned char *)in + 1;
          do {
            *out = ((((unsigned)src[1] | 0xffffff00) << 8 | (unsigned)src[0]) << 8) | (unsigned)src[-1];
            out = out + 1;
            src = src + 3;
            remainder = remainder + 1;
          } while (remainder < (unsigned)(movie->width & 3));
        }
        src = row_src + movie->source_pitch;
        y = y + 1;
        dst = dst + locked.pitch;
      } while (y < movie->height);
    }
  }
  if ((g_d3dx_texture_76e888 == D3DFMT_A1R5G5B5) && (0 < (int)movie->height)) {
    y = 0;
    do {
      row_src = src;
      row_dst = dst;
      x = 0;
      if (0 < movie->width) {
        do {
          *(unsigned short *)dst = (unsigned short)(src[0] >> 3) + (((src[2] & 0xf8) + 0x100) * 0x20 + (src[1] & 0xf8)) * 4;
          x = x + 1;
          dst = dst + 2;
          src = src + 3;
        } while (x < movie->width);
      }
      src = row_src + movie->source_pitch;
      dst = row_dst + locked.pitch;
      y = y + 1;
    } while ((int)y < (int)movie->height);
  }
  swap_index = g_movie_renderer_76e88c;
  texture = g_d3dx_76e89c[swap_index];
  if ((*(int (__stdcall **)(void *, int))((char *)*(void **)texture + VTABLE_TEXTURE_UNLOCK_RECT))(texture, 0) < 0) {
    return E_FAIL_HRESULT;
  }
  swap_index = g_movie_renderer_76e88c;
  g_movie_renderer_76e88c = (swap_index == 0);
  g_app_62ad50 = swap_index;
  return 0;
}

/* entry thunk at the original address of movie_fn_4a31b0: register inputs (ecx,s0) -> natural C movie_fn_4a31b0_n; ret 4 */
__declspec(naked) void movie_fn_4a31b0(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        call movie_fn_4a31b0_n
        add esp, 8
        ret 4
    }
}
