#include "mfl_tier2.h"
/* RenderWare skin plugin (rpskin), D3D9 part: the skin pipelines (create / destroy, 0x540540 / 0x5402d0) and the plain
   (non vertex shader) all-in-one pipeline with the device capability probe (0x542d50).  Written from the disassembly
   of MFL.exe. */

#define FALSE 0
#define rwID_SKINPLUGIN 0x116
enum { rpSKINPIPELINE_COUNT = 4 };
enum { rpSKIND3D9PIPELINEPLAIN = 0x01 };      /* pipe bit _rpSkinPipelinesCreate gets */

typedef struct SkinGlobalsD3D9 {          /* _rpSkinGlobals (0x7da640) up to the D3D9 platform part */
  char pad00[0x24];
  void *pipelines[rpSKINPIPELINE_COUNT];  /* 0x24 */
  char pad34[0x40 - 0x34];
  RwBool hardware_tnl;                    /* 0x40: the device does transform and lighting */
  RwBool vertex_shaders;                  /* 0x44: ... and vertex / pixel shaders 1.1 or later */
  RwUInt32 max_bones;                     /* 0x48: (max_vs_constants - 12) / 3 */
  RwUInt32 max_vs_constants;              /* 0x4c */
} SkinGlobalsD3D9;

typedef struct D3D9Caps {                 /* the D3DCAPS9 fields read here */
  char pad00[0x1e];
  RwUInt8 dev_caps_byte2;                 /* 0x1e: DevCaps bits 16 .. 23; bit 16 = D3DDEVCAPS_HWTRANSFORMANDLIGHT */
  char pad1f[0xc4 - 0x1f];
  RwUInt32 vertex_shader_version;         /* 0xc4 */
  RwUInt32 max_vertex_shader_const;       /* 0xc8 */
  RwUInt32 pixel_shader_version;          /* 0xcc */
} D3D9Caps;

typedef struct RxPipelinePart {           /* the RxPipeline fields written here */
  char pad00[0x2c];
  RwUInt32 plugin_id;                     /* 0x2c */
  RwUInt32 plugin_data;                   /* 0x30 */
} RxPipelinePart;

/*007da640*/ extern SkinGlobalsD3D9 _rpSkinGlobals;
/*0063a1c8*/ extern char D3D9SkinAtomicAllInOneNode[];      /* RxNodeDefinition "nodeD3D9SkinAtomicAllInOne.csl" */

/*004dbca0*/ extern D3D9Caps *RwD3D9GetCaps(void);
/*004def80*/ extern RxPipelinePart *RxPipelineCreate(void);
/*004deff0*/ extern void rxPipelineDestroy(void *pipeline);
/*004e6f40*/ extern void *RxPipelineLock(RxPipelinePart *pipeline);
/*004e7100*/ extern void *RxLockedPipeAddFragment(void *pipeline, RwUInt32 *first_index, void *node_definition, ...);
/*004e64f0*/ extern void *RxLockedPipeUnlock(void *pipeline);

RxPipelinePart *_rpSkinD3D9CreatePlainPipe(void);

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool _rpSkinPipelinesDestroy(void)
{
  void **pipeline;

  for (pipeline = _rpSkinGlobals.pipelines; pipeline < _rpSkinGlobals.pipelines + rpSKINPIPELINE_COUNT; pipeline++) {
    if (*pipeline != NULL) {
      rxPipelineDestroy(*pipeline);
      *pipeline = NULL;
    }
  }
  return TRUE;
}

/* TIER2: validated 2026-11-14 authority=diffemu */
RwBool _rpSkinPipelinesCreate(RwUInt32 pipes)
{
  RwBool ok = TRUE;
  RxPipelinePart *pipeline;

  if (pipes & rpSKIND3D9PIPELINEPLAIN) {
    pipeline = _rpSkinD3D9CreatePlainPipe();
    if (pipeline == NULL) {
      ok = FALSE;
    }
    _rpSkinGlobals.pipelines[0] = pipeline;
  }
  return ok;
}

/* Reads the device caps into the skin globals, then builds the all-in-one skin pipeline (NULL on failure). */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RxPipelinePart *_rpSkinD3D9CreatePlainPipe(void)
{
  const D3D9Caps *caps = RwD3D9GetCaps();
  RxPipelinePart *pipeline;
  void *locked;

  _rpSkinGlobals.hardware_tnl = caps->dev_caps_byte2 & 1;
  if (_rpSkinGlobals.hardware_tnl
      && (caps->vertex_shader_version & 0xffff) >= 0x101
      && (caps->pixel_shader_version & 0xffff) >= 0x101) {
    RwUInt32 constants;

    _rpSkinGlobals.vertex_shaders = TRUE;
    constants = caps->max_vertex_shader_const;
    _rpSkinGlobals.max_vs_constants = constants;
    _rpSkinGlobals.max_bones = (constants - 12) / 3;
  } else {
    _rpSkinGlobals.vertex_shaders = FALSE;
    _rpSkinGlobals.max_bones = 0;
    _rpSkinGlobals.max_vs_constants = 0;
  }

  pipeline = RxPipelineCreate();
  if (pipeline != NULL) {
    pipeline->plugin_id = rwID_SKINPLUGIN;
    pipeline->plugin_data = 1;
    locked = RxPipelineLock(pipeline);
    if (locked != NULL) {
      RxLockedPipeUnlock(RxLockedPipeAddFragment(locked, NULL, D3D9SkinAtomicAllInOneNode, NULL));
      return pipeline;
    }
    rxPipelineDestroy(pipeline);
    pipeline = NULL;
  }
  return pipeline;
}
