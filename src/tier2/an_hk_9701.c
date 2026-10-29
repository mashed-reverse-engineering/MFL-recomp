#include "mfl_tier2.h"
/*004db270*/ extern void RwD3D9SetStreamSource(int stream, void *vertex_buffer, int offset, int stride);
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 state, void *matrix);
/*004e3b30*/ extern void __cdecl _rwD3D9RenderStateFlushCache(void);
/*004e3c00*/ extern void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0062e760*/ extern int _rwD3D9LastFVFUsed;
/*0062e764*/ extern int _rwD3D9LastVertexDeclarationUsed;
/*00771a48*/ extern char g_app_771a48[64];
/*007d2240*/ extern int **g_d3d9_device;

typedef int (__stdcall *D3DMethod1)(void *device, int a);
typedef int (__stdcall *D3DMethod4)(void *device, int a, int b, void *c, int d);

#define D3DRS_LIGHTING 0x89
#define D3DTS_WORLD 0x100
#define D3DFVF_XYZ_DIFFUSE 0x42
#define D3DPT_LINELIST 2
#define D3DDEV_SET_FVF 0x164
#define D3DDEV_DRAW_PRIMITIVE_UP 0x14c
#define D3DDEV_METHOD(index) ((*g_d3d9_device)[(index) / 4])
#define RS_VERTEX_ALPHA_ENABLE 12
#define LINE_BUFFER_VERTICES 1792   /* 0x7000 bytes: 896 lines per DrawPrimitiveUP batch */

/* one debug line record (stride 0x20) */
typedef struct DebugLine {
  float from[3];
  unsigned char unknown_0c[2];
  unsigned char alpha_mask;      /* +0xe: ANDed into the alpha of the second vertex colour */
  unsigned char unknown_0f;
  float extent[3];               /* the second vertex is from - extent */
  unsigned char r, g, b, a;
} DebugLine;

typedef struct LineVertex {
  float x, y, z;
  unsigned int colour;           /* D3DCOLOR (ARGB) */
} LineVertex;

extern LineVertex g_d3d9_draw_7e0700[LINE_BUFFER_VERTICES];

/* d3d9_draw_with_transform */

/*
 * Draws `count` debug lines through DrawPrimitiveUP: sets the world transform and render state, converts every
 * 32 byte record to two vertices (from, colour) and (from - extent, alpha only) in the shared vertex buffer and
 * flushes the buffer whenever it is full. Returns 0 without drawing when count is 0, else 1.
 * The final draw submits `count` primitives from the buffer start (the original does that, not the remainder).
 */
/* TIER2: unvalidated 2026-10-29 authority=diffemu */
int d3d9_draw_with_transform(DebugLine *lines, int count)
{
  LineVertex *out = g_d3d9_draw_7e0700;
  int i;

  if (count == 0) {
    return 0;
  }
  RwD3D9SetTransform(D3DTS_WORLD, g_app_771a48);
  RwD3D9SetRenderState(D3DRS_LIGHTING, 0);
  RwD3D9SetRenderState(0x16, 1);
  if (_rwD3D9LastFVFUsed != D3DFVF_XYZ_DIFFUSE) {
    _rwD3D9LastFVFUsed = D3DFVF_XYZ_DIFFUSE;
    _rwD3D9LastVertexDeclarationUsed = -1;
    ((D3DMethod1)D3DDEV_METHOD(D3DDEV_SET_FVF))(g_d3d9_device, D3DFVF_XYZ_DIFFUSE);
  }
  RwRenderStateSet((enum RwRenderState)RS_VERTEX_ALPHA_ENABLE, (void *)1);
  for (i = 0; i < count; i++) {
    DebugLine *line = &lines[i];
    unsigned int colour = (line->a << 24) | (line->r << 16) | (line->g << 8) | line->b;

    out[0].x = line->from[0];
    out[0].y = line->from[1];
    out[0].z = line->from[2];
    out[0].colour = colour;
    out[1].x = line->from[0] - line->extent[0];
    out[1].y = line->from[1] - line->extent[1];
    out[1].z = line->from[2] - line->extent[2];
    out[1].colour = (line->alpha_mask << 24) & colour;
    out += 2;
    if (out >= &g_d3d9_draw_7e0700[LINE_BUFFER_VERTICES]) {
      RwD3D9SetStreamSource(0, 0, 0, 0);
      _rwD3D9RenderStateFlushCache();
      ((D3DMethod4)D3DDEV_METHOD(D3DDEV_DRAW_PRIMITIVE_UP))(g_d3d9_device, D3DPT_LINELIST, LINE_BUFFER_VERTICES / 2, g_d3d9_draw_7e0700, sizeof(LineVertex));
      out = g_d3d9_draw_7e0700;
    }
  }
  RwD3D9SetStreamSource(0, 0, 0, 0);
  _rwD3D9RenderStateFlushCache();
  ((D3DMethod4)D3DDEV_METHOD(D3DDEV_DRAW_PRIMITIVE_UP))(g_d3d9_device, D3DPT_LINELIST, count, g_d3d9_draw_7e0700, sizeof(LineVertex));
  RwRenderStateSet((enum RwRenderState)RS_VERTEX_ALPHA_ENABLE, (void *)0);
  return 1;
}
