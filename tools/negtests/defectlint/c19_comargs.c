// cluster-entries: d3d9_pipeline_create_default_cb_4a9ff0
// expect: class 19
// path: recomp/clusters/cl_negtest19.c
/* T-P18: DrawIndexedPrimitive with BaseVertexIndex / MinVertexIndex swapped (link 1556, render oracle prim 6712) */
/*007d2240*/ extern int **g_d3d9_device;
typedef int (__stdcall *D3DMethod6)(void *device, int a, int b, int c, int d, int e, int f);
#define D3DDEV_DRAW_INDEXED_PRIMITIVE 0x148
#define D3DDEV_METHOD(index) ((*g_d3d9_device)[(index) / 4])
typedef struct D3d9Mesh {
  int *material;
  int unknown_04;
  int vertex_shader;
  int base_index;
  int num_vertices;
  int start_index;
  int primitive_count;
} D3d9Mesh;
typedef struct D3d9InstanceHeader {
  char unknown_00[0x24];
  int primitive_type;
} D3d9InstanceHeader;
void d3d9_pipeline_create_default_cb_4a9ff0(D3d9InstanceHeader *header, int unused_a, int unused_b, unsigned int flags)
{
  D3d9Mesh *mesh;
  mesh = (D3d9Mesh *)header;
  ((D3DMethod6)D3DDEV_METHOD(D3DDEV_DRAW_INDEXED_PRIMITIVE))(g_d3d9_device, header->primitive_type, 0, mesh->base_index,
                                                            mesh->num_vertices, mesh->start_index, mesh->primitive_count);
}
