#include "mfl_tier2.h"
extern float g_data_771a90;
extern float g_data_771a94;
/*004db5d0*/ extern void RwD3D9GetTransform();
/*004e3c00*/ extern void RwD3D9SetRenderState();
/*004e55d0*/ extern void FUN_004e55d0();
/*004e3c70*/ extern void RwD3D9SetTextureStageState();
/*004e3cd0*/ extern void RwD3D9SetSamplerState();
/*004db470*/ extern void RwD3D9SetTransform();
/*004db430*/ extern void _rwD3D9DrawIndexedPrimitive();

#define D3DRS_TEXTUREFACTOR (0x3c)
#define TEXTURE_FACTOR_ARGB (0x96ffffff)
#define D3DTS_PROJECTION (3)
#define D3DTS_TEXTURE1 (0x11)
#define D3DTSS_COLOROP (1)
#define D3DTSS_COLORARG1 (2)
#define D3DTSS_COLORARG2 (3)
#define D3DTSS_ALPHAOP (4)
#define D3DTSS_TEXCOORDINDEX (0xb)
#define D3DTSS_TEXTURETRANSFORMFLAGS (0x18)
#define D3DTTFF_COUNT3_PROJECTED (0x103)
#define D3DTSS_TCI_CAMERASPACEPOSITION (0x20000)
#define D3DSAMP_ADDRESSU (1)
#define D3DSAMP_ADDRESSV (2)

#define FL(o) (*(float *)((char *)matrix_buf + (o)))
#define FI(o) (*(int *)((char *)matrix_buf + (o)))

/*004aa740*/
/* TIER2: unvalidated 2026-10-12 authority=diffemu */
/* Draws one indexed primitive batch with two texture stages (inferred purpose).
 * Builds a texture matrix in matrix_buf from the current projection matrix
 * (matrix_buf[0..15] = texture matrix, matrix_buf[16..31] = projection read back),
 * binds stage0_texture and stage1_texture, sets the stage states, draws the batch,
 * then resets stage 1 and stage 0 texcoord state. No return value.
 * mesh: source whose int at +0xc is the primitive type (inferred).
 * draw_range: source with base vertex (+0x14), vertex count (+0x18), start index (+0x1c)
 *   and primitive count (+0x20) (inferred).
 * stage0_texture / stage1_texture: texture handles for stages 0 and 1 (inferred).
 * a3, a6, a7, a8: unused. */
/* TIER2: unvalidated 2026-10-22 authority=diffemu */
void FUN_004aa740(char *mesh, char *draw_range, int a3, int stage0_texture, int stage1_texture, int a6, int a7, int a8)
{
  int matrix_buf[0x40];

  /* Fill the matrix constants: 0.5 scale on x, -0.5 on y, offsets and 1.0 terms (inferred). */
  FL(0x00) = 0.5f;
  FI(0x04) = 0;
  FI(0x08) = 0;
  FI(0x0c) = 0;
  FI(0x10) = 0;
  FL(0x14) = -0.5f;
  FI(0x18) = 0;
  FI(0x1c) = 0;
  FL(0x20) = 0.5f;
  FL(0x24) = 0.5f;
  FL(0x28) = 1.0f;
  FI(0x2c) = 0;
  FI(0x30) = 0;
  FI(0x34) = 0;
  FI(0x38) = 0;
  FL(0x3c) = 1.0f;
  /* Read the projection matrix and fold it into the texture matrix (inferred). */
  RwD3D9GetTransform(D3DTS_PROJECTION, (char *)&matrix_buf[16]);
  FL(0x20) = g_data_771a90 + FL(0x20);
  FL(0x24) = g_data_771a94 + FL(0x24);
  FL(0x00) = FL(0x40) * FL(0x00);
  FL(0x14) = FL(0x54) * FL(0x14);
  RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, TEXTURE_FACTOR_ARGB);
  /* Stage 0 setup: bind texture, stage state and sampler addressing. */
  FUN_004e55d0(stage0_texture, 0);
  RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, 2);
  RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, 4);
  RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, 0);
  RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, 1);
  RwD3D9SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
  RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
  RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
  RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
  /* Stage 1 setup: bind texture, stage state, camera-space texcoords and the texture matrix. */
  FUN_004e55d0(stage1_texture, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, 2);
  RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, 0xe);
  RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3_PROJECTED);
  RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
  RwD3D9SetTransform(D3DTS_TEXTURE1, (char *)&matrix_buf[0]);
  RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSU, 3);
  RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSV, 3);
  /* Draw the batch. Min vertex index is passed as 0. */
  _rwD3D9DrawIndexedPrimitive(*(int *)((char *)mesh + 0xc), *(int *)((char *)draw_range + 0x14), 0, *(int *)((char *)draw_range + 0x18), *(int *)((char *)draw_range + 0x1c), *(int *)((char *)draw_range + 0x20));
  /* Cleanup: reset stage 0 texcoords, unbind stage 1 texture, reset stage 1 ops and texture transform. */
  RwD3D9SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
  FUN_004e55d0(0, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, 1);
  RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
}
