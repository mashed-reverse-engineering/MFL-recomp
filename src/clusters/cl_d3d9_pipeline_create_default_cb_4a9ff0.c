// cluster-entries: d3d9_pipeline_create_default_cb_4a9ff0
/* Installed cluster d3d9_pipeline_create_default_cb_4a9ff0 (tools/cluster_install.py from output/cluster_pilot_pg/d3d9_pipeline_create_default_cb_4a9ff0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004a6d30*/ extern void __cdecl matrix_transpose_row_copy(void *dst, RwMatrix *src);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *frame);
/*004db2d0*/ extern void _rwD3D9SetStreams(void *streams, RwBool use_offsets);
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 state, void *matrix);
/*004e3b30*/ extern void __cdecl _rwD3D9RenderStateFlushCache(void);
/*004e3c00*/ extern void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value);
/*004e3c70*/ extern void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value);
/*004e3d10*/ extern RwBool __cdecl RwD3D9SetSurfaceProperties(void *surface, void *colour, RwUInt32 flags);
/*004e55d0*/ extern void FUN_004e55d0(int texture, int stage);
/*004fc623*/ extern void __stdcall thunk_FUN_004fc616(void *dst, void *src);   /* tail jump into D3DX: ret 8 */
/*0062e760*/ extern int _rwD3D9LastFVFUsed;
/*0062e764*/ extern int _rwD3D9LastVertexDeclarationUsed;
/*0062e768*/ extern int _rwD3D9LastVertexShaderUsed;
/*0062e76c*/ extern int _rwD3D9LastPixelShaderUsed;
/*0062e770*/ extern int _rwD3D9LastIndexBufferUsed;
/*00771a88*/ extern int g_data_771a88;
/*00771a8c*/ extern struct RwFrame **g_data_771a8c;
/*007d2240*/ extern int **g_d3d9_device;

typedef int (__stdcall *D3DMethod1)(void *device, int a);
typedef int (__stdcall *D3DMethod6)(void *device, int a, int b, int c, int d, int e, int f);

/* Direct3D 9 values the instance render uses (render states are D3DRS_*, stage states D3DTSS_* / D3DTOP_* / D3DTA_*) */
#define D3DRS_LIGHTING 0x89
#define D3DRS_COLORVERTEX 0x8d
#define D3DTS_WORLD 0x100
#define D3DTSS_COLOROP 1
#define D3DTSS_COLORARG1 2
#define D3DTSS_COLORARG2 3
#define D3DTSS_ALPHAOP 4
#define D3DTSS_ALPHAARG1 5
#define D3DTSS_ALPHAARG2 6
#define D3DTOP_MODULATE 4
#define D3DTA_DIFFUSE 0
#define D3DTA_CURRENT 1
#define D3DTA_TEXTURE 2
/* geometry flags tested by the callback */
#define GEOMETRY_NORMALS 0x10  /* lighting is enabled when the geometry has normals */
#define GEOMETRY_LIGHT 0x20    /* set the material's surface properties */
/* byte offsets into the device vtable (IDirect3DDevice9) */
#define D3DDEV_SET_VERTEX_DECLARATION 0x15c
#define D3DDEV_SET_INDICES 0x1a0
#define D3DDEV_SET_VERTEX_SHADER 0x170
#define D3DDEV_SET_PIXEL_SHADER 0x1ac
#define D3DDEV_DRAW_INDEXED_PRIMITIVE 0x148
#define D3DDEV_METHOD(index) ((*g_d3d9_device)[(index) / 4])

/* one mesh of an instance header (stride 0x24) */
typedef struct D3d9Mesh {
  int *material;            /* word 0: texture; +4: RGBA colour; +0xc: surface properties */
  int unknown_04;
  int vertex_shader;
  int min_index;
  int num_vertices;
  int start_index;
  int primitive_count;
  int unknown_1c[2];
} D3d9Mesh;

/* RenderWare D3D9 instance header (RxD3D9ResEntryHeader-like) */
typedef struct D3d9InstanceHeader {
  char unknown_00[0x1c];
  int mesh_count;
  int index_buffer;
  int primitive_type;
  char vertex_streams[0x20];   /* RxD3D9VertexStream[] */
  int use_offsets;
  int vertex_declaration;
  char unknown_50[0x10];
  D3d9Mesh meshes[1];          /* mesh_count entries at +0x60 */
} D3d9InstanceHeader;

/* d3d9_pipeline_create_default_cb_4a9ff0 */

/* Render callback of the default D3D9 pipeline (pushed by d3d9_pipeline_create_default): binds the instance's index buffer, vertex
   streams and vertex declaration (each only when it differs from the cached one), then for every mesh sets the material, a plain
   modulated texture stage 0, the mesh's vertex shader, no pixel shader, and draws the mesh once per frame in g_data_771a8c (world
   matrix = the transposed LTM of that frame). */
void d3d9_pipeline_create_default_cb_4a9ff0(D3d9InstanceHeader *header, int unused_a, int unused_b, unsigned int flags)
{
  RwMatrix world;
  int meshes_left;
  int frames_left;
  D3d9Mesh *mesh;
  int *material;
  int texture;

  RwD3D9SetRenderState(D3DRS_LIGHTING, (flags & GEOMETRY_NORMALS) != 0);
  RwD3D9SetRenderState(D3DRS_COLORVERTEX, 1);
  if (_rwD3D9LastIndexBufferUsed != header->index_buffer) {
    _rwD3D9LastIndexBufferUsed = header->index_buffer;
    ((D3DMethod1)D3DDEV_METHOD(D3DDEV_SET_INDICES))(g_d3d9_device, header->index_buffer);
  }
  _rwD3D9SetStreams(header->vertex_streams, header->use_offsets);
  if (_rwD3D9LastVertexDeclarationUsed != header->vertex_declaration) {
    _rwD3D9LastFVFUsed = -1;
    _rwD3D9LastVertexDeclarationUsed = header->vertex_declaration;
    ((D3DMethod1)D3DDEV_METHOD(D3DDEV_SET_VERTEX_DECLARATION))(g_d3d9_device, header->vertex_declaration);
  }
  meshes_left = header->mesh_count;
  if (meshes_left != 0) {
    mesh = header->meshes;
    do {
      frames_left = g_data_771a88;
      material = mesh->material;
      texture = material[0];
      if ((flags & GEOMETRY_LIGHT) != 0) {
        RwD3D9SetSurfaceProperties(material + 3, material + 1, flags);
      }
      FUN_004e55d0(texture, 0);
      RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
      RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
      RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
      RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
      RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
      RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
      if (_rwD3D9LastVertexShaderUsed != mesh->vertex_shader) {
        _rwD3D9LastVertexShaderUsed = mesh->vertex_shader;
        ((D3DMethod1)D3DDEV_METHOD(D3DDEV_SET_VERTEX_SHADER))(g_d3d9_device, mesh->vertex_shader);
      }
      if (_rwD3D9LastPixelShaderUsed != 0) {
        _rwD3D9LastPixelShaderUsed = 0;
        ((D3DMethod1)D3DDEV_METHOD(D3DDEV_SET_PIXEL_SHADER))(g_d3d9_device, 0);
      }
      for (; frames_left != 0; frames_left--) {
        matrix_transpose_row_copy(&world, RwFrameGetLTM(g_data_771a8c[frames_left - 1]));
        thunk_FUN_004fc616(&world, &world);
        RwD3D9SetTransform(D3DTS_WORLD, &world);
        _rwD3D9RenderStateFlushCache();
        ((D3DMethod6)D3DDEV_METHOD(D3DDEV_DRAW_INDEXED_PRIMITIVE))(g_d3d9_device, header->primitive_type, 0, mesh->min_index,
                                                                  mesh->num_vertices, mesh->start_index, mesh->primitive_count);
      }
      mesh++;
      meshes_left--;
    } while (meshes_left != 0);
  }
}

