#include "mfl_tier2.h"
/*004db430*/ extern void _rwD3D9DrawIndexedPrimitive(RwUInt32 p0, RwInt32 p1, RwUInt32 p2, RwUInt32 p3, RwUInt32 p4, RwUInt32 p5);
/*004db470*/ extern RwBool RwD3D9SetTransform(RwUInt32 p0, void *p1);
/*004db5d0*/ extern void RwD3D9GetTransform(RwUInt32 p0, void *p1);
/*004e3c00*/ extern void RwD3D9SetRenderState(RwUInt32 p0, RwUInt32 p1);
/*004e3c70*/ extern void RwD3D9SetTextureStageState(RwUInt32 p0, RwUInt32 p1, RwUInt32 p2);
/*004e3cd0*/ extern void RwD3D9SetSamplerState(RwUInt32 p0, RwUInt32 p1, RwUInt32 p2);
/*004e55d0*/ extern void FUN_004e55d0(undefined4 p0, undefined4 p1);
/* D3DRS_TEXTUREFACTOR: render state that feeds the constant colour to the blend */
#define RS_TEXTURE_FACTOR (0x3c)
/* Texture factor colour: alpha 0x96 (about 59%), rgb white (inferred: shadow strength) */
#define TEXTURE_FACTOR_SHADOW (0x96ffffff)
/* D3DTS_TEXTURE1: transform slot for stage 1 */
#define TRANSFORM_TEXTURE1 (0x11)
/* D3DTSS_TCI_CAMERASPACEPOSITION: stage 1 texcoords come from camera-space position */
#define TEXCOORD_CAMERASPACE_POSITION (0x20000)
/* D3DTTFF_PROJECTED | D3DTTFF_COUNT3: projective 3-component texture transform */
#define TEXTURE_TRANSFORM_PROJECTED_COUNT3 (0x103)
/* FUN_004aa530 */
/* Draws one primitive range as a projective overlay pass (inferred: shadow/decal on top of the
 * already drawn geometry). Builds a texture matrix from the current projection, sets stage 0
 * (base texture, modulated with the texture factor) and stage 1 (detail texture, projected
 * camera-space texcoords), draws the range, then resets stage 1 and stage 0 state.
 *   geometry:       primitive type at +0xc
 *   index_range:    draw range: base vertex +0x14, vertex count +0x18, start index +0x1c, primitive count +0x20
 *   base_texture:   texture for stage 0
 *   detail_texture: texture for stage 1
 *   unused_3, unused_6: never read
 * Returns nothing. */
void __cdecl FUN_004aa530(char *geometry,char *index_range,char *unused_3,undefined4 base_texture,undefined4 detail_texture, char *unused_6)
{
  struct {
    float local_80 [21];
    float local_2c;
    int pad_top[10];
  } transform_block;

  /* Texture matrix: bias 0.5/-0.5 (maps clip space to texture space) in elements [0..15];
     the projection (D3DTS_PROJECTION, read into [16..31]) scales its x and y terms. */
  transform_block.local_80[0] = 0.5;
  transform_block.local_80[1] = 0.0;
  transform_block.local_80[2] = 0.0;
  transform_block.local_80[3] = 0.0;
  transform_block.local_80[4] = 0.0;
  transform_block.local_80[5] = -0.5;
  transform_block.local_80[6] = 0.0;
  transform_block.local_80[7] = 0.0;
  transform_block.local_80[8] = 0.5;
  transform_block.local_80[9] = 0.5;
  transform_block.local_80[10] = 1.0;
  transform_block.local_80[0xb] = 0.0;
  transform_block.local_80[0xc] = 0.0;
  transform_block.local_80[0xd] = 0.0;
  transform_block.local_80[0xe] = 0.0;
  transform_block.local_80[0xf] = 1.0;
  RwD3D9GetTransform(3,transform_block.local_80 + 0x10);
  transform_block.local_80[0] = transform_block.local_80[0x10] * transform_block.local_80[0];
  transform_block.local_80[5] = transform_block.local_2c * transform_block.local_80[5];
  /* Stage 0 setup: base texture, texture factor colour */
  RwD3D9SetRenderState(RS_TEXTURE_FACTOR,TEXTURE_FACTOR_SHADOW);
  FUN_004e55d0(base_texture,0);
  RwD3D9SetTextureStageState(0,2,2);
  RwD3D9SetTextureStageState(0,1,4);
  RwD3D9SetTextureStageState(0,3,0);
  RwD3D9SetTextureStageState(0,4,1);
  RwD3D9SetTextureStageState(0,0x18,0);
  RwD3D9SetTextureStageState(0,0xb,0);
  RwD3D9SetSamplerState(0,1,1);
  RwD3D9SetSamplerState(0,2,1);
  FUN_004e55d0(detail_texture,1);
  /* Stage 1 setup: detail texture, projected texcoords from the texture matrix */
  RwD3D9SetTextureStageState(1,2,1);
  RwD3D9SetTextureStageState(1,3,2);
  RwD3D9SetTextureStageState(1,1,0xe);
  RwD3D9SetTextureStageState(1,4,1);
  RwD3D9SetTextureStageState(1,0x18,TEXTURE_TRANSFORM_PROJECTED_COUNT3);
  RwD3D9SetTextureStageState(1,0xb,TEXCOORD_CAMERASPACE_POSITION);
  RwD3D9SetTransform(TRANSFORM_TEXTURE1,transform_block.local_80);
  RwD3D9SetSamplerState(1,1,3);
  RwD3D9SetSamplerState(1,2,3);
  /* Draw the indexed range with the stage 0 and stage 1 state set above */
  _rwD3D9DrawIndexedPrimitive
            (*(RwUInt32 *)(geometry + 0xc),*(RwInt32 *)(index_range + 0x14),0,
             *(RwUInt32 *)(index_range + 0x18),*(RwUInt32 *)(index_range + 0x1c),
             *(RwUInt32 *)(index_range + 0x20));
  /* Reset stage 0 texcoord index, then stage 1 state and texture (0 = unbound) */
  RwD3D9SetTextureStageState(0,0xb,0);
  FUN_004e55d0(0,1);
  RwD3D9SetTextureStageState(1,1,1);
  RwD3D9SetTextureStageState(1,4,1);
  RwD3D9SetTextureStageState(1,0x18,0);
  return;
}
