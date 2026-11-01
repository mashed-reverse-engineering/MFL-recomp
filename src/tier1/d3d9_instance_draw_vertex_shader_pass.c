#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00771a90*/ extern float g_data_771a90;
/*00771aa0*/ extern undefined4 g_data_771aa0;
/*004a6bf0*/ extern void __cdecl camera_view_matrix_from_frame(float *view_out, void *camera);
/*004a6d30*/ extern void __cdecl matrix_transpose_row_copy(float *dst, void *src);
/*004a6da0*/ extern void __cdecl camera_view_params_pack(float *out, void *camera);
/*004fc3da*/ extern void __stdcall thunk_FUN_004fc3cd(float *dst, float *a, float *b);
/*004dbc50*/ extern void __cdecl rwD3D9SetVertexShaderConstant(RwUInt32 first_register, float *values, RwUInt32 vector_count);
/*004db1f0*/ extern void __cdecl rwD3D9SetVertexShader(undefined4 p0);
/*004db430*/ extern void _rwD3D9DrawIndexedPrimitive(RwUInt32 p0, RwInt32 p1, RwUInt32 p2, RwUInt32 p3, RwUInt32 p4, RwUInt32 p5);
/*004e3c00*/ extern void RwD3D9SetRenderState(RwUInt32 p0, RwUInt32 p1);
/*004e3c50*/ extern void RwD3D9GetRenderState(RwUInt32 p0, void *p1);
/*004e3c70*/ extern void RwD3D9SetTextureStageState(RwUInt32 p0, RwUInt32 p1, RwUInt32 p2);
/*004e3cd0*/ extern void RwD3D9SetSamplerState(RwUInt32 p0, RwUInt32 p1, RwUInt32 p2);
/*004e55d0*/ extern void FUN_004e55d0(undefined4 p0, undefined4 p1);
/* RwCamera::farPlane (the macro list of mfl_tier2.h has the next field, fogPlane, at +0x88) */
#define RwCamera_farPlane(p) (*(RwReal *)((char *)(p) + 0x84))
/* D3DRS_FOGENABLE, D3DRS_FOGTABLEMODE: both are switched off for the pass and restored afterwards */
#define RS_FOG_ENABLE (0x1c)
#define RS_FOG_TABLE_MODE (0x23)
/* vertex shader constant registers written by this pass */
#define VS_REG_WORLD_VIEW_PROJ (0)
#define VS_REG_WORLD (4)
#define VS_REG_VIEW (8)
#define VS_REG_CONSTANTS (0xc)
#define VS_REG_FOG (0xd)
#define VS_REG_SHADER_PARAM (0xe)
/* d3d9_instance_draw_vertex_shader_pass (FUN_004aa2b0) */
/* Draws one primitive range with the custom vertex shader (inferred from the constant registers it fills): uploads the world-view-projection, world and view matrices,
 * the constant vector (1, 0.5, 0.25, 0) and the fog scale (far plane, 1 / (far - fog plane)), switches fog off, binds the two textures with modulate/clamp
 * texture stage state, draws the range, then unbinds stage 1 and restores the two fog render states.
 *   range_owner:  primitive type at +0xc
 *   index_range:  draw range: base vertex +0x14, vertex count +0x18, start index +0x1c, primitive count +0x20
 *   world_matrix: matrix of the instance
 *   texture_0, texture_1: textures of stages 0 and 1
 *   unused_6: never read
 * Returns nothing. */
void __cdecl d3d9_instance_draw_vertex_shader_pass(char *range_owner, char *index_range, float *world_matrix, undefined4 texture_0, undefined4 texture_1, char *unused_6)
{
  float constants[4];
  float fog[4];
  float world[16];
  float view[16];
  float view_proj[16];
  float params[16];
  float world_view_proj[16];
  void *camera;
  float far_plane;
  float fog_plane;
  int saved_fog_enable;
  int saved_fog_table_mode;

  camera = RwCameraGetCurrentCamera();
  constants[0] = 1.0f;
  constants[1] = 0.5f;
  constants[2] = 0.25f;
  constants[3] = 0.0f;
  fog_plane = RwCamera_fogPlane(camera);
  far_plane = RwCamera_farPlane(camera);
  fog[0] = far_plane;
  fog[1] = 1.0f / (far_plane - fog_plane);
  fog[2] = 0.0f;
  fog[3] = 0.0f;
  camera_view_matrix_from_frame(view, camera);
  matrix_transpose_row_copy(world, world_matrix);
  camera_view_params_pack(params, camera);
  thunk_FUN_004fc3cd(view_proj, view, world);
  thunk_FUN_004fc3cd(world_view_proj, params, view_proj);
  rwD3D9SetVertexShaderConstant(VS_REG_WORLD_VIEW_PROJ, world_view_proj, 4);
  rwD3D9SetVertexShaderConstant(VS_REG_WORLD, world, 4);
  rwD3D9SetVertexShaderConstant(VS_REG_VIEW, view, 4);
  rwD3D9SetVertexShaderConstant(VS_REG_CONSTANTS, constants, 1);
  rwD3D9SetVertexShaderConstant(VS_REG_FOG, fog, 1);
  rwD3D9SetVertexShaderConstant(VS_REG_SHADER_PARAM, &g_data_771a90, 1);
  rwD3D9SetVertexShader(g_data_771aa0);
  RwD3D9GetRenderState(RS_FOG_ENABLE, &saved_fog_enable);
  RwD3D9GetRenderState(RS_FOG_TABLE_MODE, &saved_fog_table_mode);
  RwD3D9SetRenderState(RS_FOG_ENABLE, 1);
  RwD3D9SetRenderState(RS_FOG_TABLE_MODE, 0);
  FUN_004e55d0(texture_0, 0);
  RwD3D9SetTextureStageState(0, 2, 2);
  RwD3D9SetTextureStageState(0, 1, 2);
  RwD3D9SetTextureStageState(0, 4, 1);
  RwD3D9SetTextureStageState(0, 0x18, 0);
  RwD3D9SetSamplerState(0, 1, 1);
  RwD3D9SetSamplerState(0, 2, 1);
  FUN_004e55d0(texture_1, 1);
  RwD3D9SetTextureStageState(1, 2, 2);
  RwD3D9SetTextureStageState(1, 1, 2);
  RwD3D9SetTextureStageState(1, 4, 1);
  RwD3D9SetTextureStageState(1, 0x18, 0x100);
  RwD3D9SetSamplerState(1, 1, 3);
  RwD3D9SetSamplerState(1, 2, 3);
  _rwD3D9DrawIndexedPrimitive(*(RwUInt32 *)(range_owner + 0xc), *(RwInt32 *)(index_range + 0x14), 0, *(RwUInt32 *)(index_range + 0x18), *(RwUInt32 *)(index_range + 0x1c), *(RwUInt32 *)(index_range + 0x20));
  FUN_004e55d0(0, 1);
  RwD3D9SetTextureStageState(1, 1, 1);
  RwD3D9SetTextureStageState(1, 4, 1);
  RwD3D9SetTextureStageState(1, 0x18, 0);
  RwD3D9SetRenderState(RS_FOG_ENABLE, saved_fog_enable);
  RwD3D9SetRenderState(RS_FOG_TABLE_MODE, saved_fog_table_mode);
}
