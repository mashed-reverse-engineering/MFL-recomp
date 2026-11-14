#include "mfl_tier2.h"
/* RenderWare material effects plugin (rpmatfx): the atomic and world sector extension readers (0x550040, 0x550120).
   The stream holds one RwInt32: non-zero = the object renders through the material effects pipeline.  Written from
   the disassembly of MFL.exe. */

/*007da72c*/ extern RwInt32 MatFXAtomicDataOffset;          /* plugin data offset in RpAtomic: RwBool effects enabled */
/*007da730*/ extern RwInt32 MatFXWorldSectorDataOffset;     /* ... in RpWorldSector */

/*004dc8d0*/ extern void *RwStreamReadInt32(void *stream, RwInt32 *ints, RwUInt32 num_bytes);
/*005540a0*/ extern void *_rpMatFXPipelineAtomicSetup(void *atomic);
/*005540b0*/ extern void *rpMatFXPipelineWorldSectorSetup(void *sector);

/*007da734*/ extern void *MatFXAtomicPipe;
/*007da738*/ extern void *MatFXWorldSectorPipe;
/*007da73c*/ extern void *BumpMapPixelShader;
/*007da740*/ extern void *EnvMapPixelShader;
/*007da744*/ extern void *EnvMapNoBaseTexturePixelShader;
/*007da748*/ extern void *BumpEnvMapPixelShader;
/*007da74c*/ extern void *BumpModulatedEnvMapPixelShader;
/*007da750*/ extern void *BumpModulatedEnvMapNoAlphaPixelShader;
/*004deff0*/ extern void rxPipelineDestroy(void *pipeline);
/*004dbc40*/ extern void RwD3D9DeletePixelShader(void *shader);

#define MATFX_ENABLED(object, offset) ((RwBool *)((char *)(object) + (offset)))

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *MatFXAtomicStreamRead(void *stream, RwInt32 length, void *atomic, RwInt32 offset, RwInt32 size)
{
  RwInt32 effects;

  if (RwStreamReadInt32(stream, &effects, sizeof(effects)) == NULL) {
    return NULL;
  }
  if (effects && !*MATFX_ENABLED(atomic, MatFXAtomicDataOffset) && _rpMatFXPipelineAtomicSetup(atomic) != NULL) {
    *MATFX_ENABLED(atomic, MatFXAtomicDataOffset) = TRUE;
  }
  return stream;
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *MatFXWorldSectorStreamRead(void *stream, RwInt32 length, void *sector, RwInt32 offset, RwInt32 size)
{
  RwInt32 effects;

  if (RwStreamReadInt32(stream, &effects, sizeof(effects)) == NULL) {
    return NULL;
  }
  if (effects && !*MATFX_ENABLED(sector, MatFXWorldSectorDataOffset) && rpMatFXPipelineWorldSectorSetup(sector) != NULL) {
    *MATFX_ENABLED(sector, MatFXWorldSectorDataOffset) = TRUE;
  }
  return stream;
}

#define RELEASE(object, destroy) do { if ((object) != NULL) { destroy(object); (object) = NULL; } } while (0)

/* Plugin close (0x553fd0): the two material effects pipelines and the six pixel shaders. */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rpMatFXPipelinesDestroy(void)
{
  RELEASE(MatFXAtomicPipe, rxPipelineDestroy);
  RELEASE(MatFXWorldSectorPipe, rxPipelineDestroy);
  RELEASE(BumpMapPixelShader, RwD3D9DeletePixelShader);
  RELEASE(EnvMapPixelShader, RwD3D9DeletePixelShader);
  RELEASE(EnvMapNoBaseTexturePixelShader, RwD3D9DeletePixelShader);
  RELEASE(BumpEnvMapPixelShader, RwD3D9DeletePixelShader);
  RELEASE(BumpModulatedEnvMapPixelShader, RwD3D9DeletePixelShader);
  RELEASE(BumpModulatedEnvMapNoAlphaPixelShader, RwD3D9DeletePixelShader);
  return TRUE;
}
