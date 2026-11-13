#include "mfl_tier2.h"
/* RenderWare patch plugin (rppatch): engine open / close, the default LOD callback and the D3D9 pipelines
   (0x559d40 .. 0x55a0c0).  Written from the disassembly of MFL.exe.  RpPatchPluginAttach, which takes the addresses
   of the open / close callbacks, is in an_h_patchplugin.c. */

#define FALSE 0
#define rwID_SKINPLUGIN 0x116
#define rwID_MATERIALEFFECTSPLUGIN 0x120
#define rwID_PATCHPLUGIN 0x123
#define PATCH_FREELIST_HINT 0x40123

enum {                                    /* pipeline types _rpPatchPipelinesCreate gets */
  rpPATCHTYPEGENERIC = 0x01,
  rpPATCHTYPESKIN = 0x02,
  rpPATCHTYPEMATFX = 0x04
};
enum { rpPATCHPIPELINE_COUNT = 4 };      /* generic, skin, matfx, skin + matfx */

typedef struct RpPatchLODRange {
  RwUInt32 min_lod;                       /* refinement at max_range and beyond */
  RwInt32 max_lod;                        /* refinement at min_range and nearer (the exe converts it as a signed int) */
  RwReal min_range;
  RwReal max_range;
} RpPatchLODRange;

typedef struct PatchEngineData {          /* engine plugin data (RwEngineInstance + engine_offset), 0x18 bytes */
  RpPatchLODRange range;
  RwReal lod_slope;                       /* (min_lod - max_lod) / (max_range - min_range) */
  RwReal inv_range;                       /* 1 / (max_range - min_range) */
} PatchEngineData;

typedef struct PatchGlobals {             /* _rpPatchGlobals */
  RwInt32 engine_offset;
  RwInt32 atomic_offset;
  RwInt32 geometry_offset;
  RwInt32 module_globals_offset;
  RwInt32 module_num_instances;
  void *atomic_free_list;
  void *geometry_free_list;
  void *pipelines[rpPATCHPIPELINE_COUNT];
  RwBool hardware_tnl;                    /* the device does transform and lighting */
  RwBool vertex_shaders;                  /* ... and vertex shaders 1.1 or later */
} PatchGlobals;

typedef struct D3D9Caps {                 /* the D3DCAPS9 fields read here */
  char pad00[0x1e];
  RwUInt8 dev_caps_byte2;                 /* 0x1e: DevCaps bits 16 .. 23; bit 16 = D3DDEVCAPS_HWTRANSFORMANDLIGHT */
  char pad1f[0xc4 - 0x1f];
  RwUInt32 vertex_shader_version;         /* 0xc4 */
} D3D9Caps;

typedef struct D3D9InstanceNodeData {     /* private data of the patch all-in-one node */
  RwBool active;
  void *instance_callback;
  void *reinstance_callback;
  void *render_callback;
} D3D9InstanceNodeData;

typedef struct RxPipelineNodePart {       /* the RxPipelineNode field read here */
  char pad00[0x14];
  D3D9InstanceNodeData *private_data;     /* 0x14 */
} RxPipelineNodePart;

typedef struct RxPipelinePart {           /* the RxPipeline fields written here */
  char pad00[0x2c];
  RwUInt32 plugin_id;                     /* 0x2c */
  RwUInt32 plugin_data;                   /* 0x30 */
} RxPipelinePart;

typedef struct RwMatrixFull {
  RwV3d right; RwUInt32 flags;
  RwV3d up; RwUInt32 pad1;
  RwV3d at; RwUInt32 pad2;
  RwV3d pos; RwUInt32 pad3;
} RwMatrixFull;
#define RwCamera_viewMatrix(camera) ((RwMatrixFull *)((char *)(camera) + 0x20))

/*007d2128*/ extern char *RwEngineInstance;
/*007da860*/ extern PatchGlobals _rpPatchGlobals;
/*0063a310*/ extern RwInt32 _rpPatchAtomicFreeListBlockSize;
/*0063a314*/ extern RwInt32 _rpPatchAtomicFreeListPreallocBlocks;
/*0063a318*/ extern RwInt32 _rpPatchGeometryFreeListBlockSize;
/*0063a31c*/ extern RwInt32 _rpPatchGeometryFreeListPreallocBlocks;
/*007da818*/ extern char _rpPatchAtomicFreeList[];        /* in-place space of the free lists */
/*007da83c*/ extern char _rpPatchGeometryFreeList[];
/*0063a330*/ extern char D3D9PatchAtomicAllInOneNode[];   /* RxNodeDefinition "PatchAtomic.csl" */

/*004d2f50*/ extern RwInt32 RwEngineGetPluginOffset(RwUInt32 plugin_id);
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entry_size, RwInt32 entries_per_block, RwInt32 alignment,
                                                              RwInt32 prealloc_blocks, void *space, RwUInt32 hint);
/*004e92b0*/ extern void RwFreeListDestroy(void *free_list);
/*004d0ff0*/ extern RwMatrixFull *RwFrameGetLTM(void *frame);
/*004d3cd0*/ extern RwMatrixFull *RwMatrixMultiply(RwMatrixFull *out, const RwMatrixFull *a, const RwMatrixFull *b);
/*004dbca0*/ extern D3D9Caps *RwD3D9GetCaps(void);
/*004def80*/ extern RxPipelinePart *RxPipelineCreate(void);
/*004deff0*/ extern void rxPipelineDestroy(void *pipeline);
/*004e6f40*/ extern void *RxPipelineLock(RxPipelinePart *pipeline);
/*004e7100*/ extern void *RxLockedPipeAddFragment(void *pipeline, RwUInt32 *first_index, void *node_definition, ...);
/*004e64f0*/ extern void *RxLockedPipeUnlock(void *pipeline);
/*004faeb0*/ extern void rxD3D9DefaultRenderCallback(void);
/*00405e40*/ extern void *stub_return_zero(void);        /* the skin / matfx pipeline constructors of this build: return NULL */

#define PATCH_ENGINE_DATA() ((PatchEngineData *)(RwEngineInstance + _rpPatchGlobals.engine_offset))

/* RenderWare's own inline-assembly real -> integer conversion (truncating fistp, 0x80000000 for NaN / out of range),
   inlined by the exe; see recomp/cheat_allow.txt (RwFastRealToUInt32_r). */
static RwUInt32 RwFastRealToUInt32_r(RwReal x)
{
#if defined(NOASM)
  return (x >= -2147483648.0f && x < 2147483648.0f) ? (RwUInt32)(RwInt32)x : 0x80000000u;
#else
  RwUInt16 savemode;
  RwUInt16 workmode;
  RwInt32 res;

  __asm
  {
    fnstcw savemode
    fld dword ptr [x]
    mov ax, savemode
    or ah, 0ch
    mov workmode, ax
    fldcw workmode
    fistp dword ptr [res]
    fldcw savemode
  }
  return (RwUInt32)res;
#endif
}

RwBool RpPatchSetDefaultLODCallBackRange(RpPatchLODRange *range);
RwBool _rpPatchPipelinesCreate(RwUInt32 type);
RwBool rpPatchPipelinesDestroy(void);
void *_rwD3D9PatchPipelineCreateGeneric(void);

void *PatchEngineOpen(void *instance, RwInt32 offset, RwInt32 size)
{
  RpPatchLODRange range;
  RwUInt32 type;

  if (_rpPatchGlobals.module_num_instances == 0) {
    type = rpPATCHTYPEGENERIC;
    if (RwEngineGetPluginOffset(rwID_SKINPLUGIN) != -1) {
      type = rpPATCHTYPEGENERIC | rpPATCHTYPESKIN;
    }
    if (RwEngineGetPluginOffset(rwID_MATERIALEFFECTSPLUGIN) != -1) {
      type |= rpPATCHTYPEMATFX;
    }
    _rpPatchPipelinesCreate(type);
    _rpPatchGlobals.atomic_free_list = RwFreeListCreateAndPreallocateSpace(0xc, _rpPatchAtomicFreeListBlockSize, 4,
      _rpPatchAtomicFreeListPreallocBlocks, _rpPatchAtomicFreeList, PATCH_FREELIST_HINT);
    _rpPatchGlobals.geometry_free_list = RwFreeListCreateAndPreallocateSpace(8, _rpPatchGeometryFreeListBlockSize, 4,
      _rpPatchGeometryFreeListPreallocBlocks, _rpPatchGeometryFreeList, PATCH_FREELIST_HINT);
  }
  _rpPatchGlobals.module_num_instances++;

  range.min_lod = 4;
  range.max_lod = 12;
  range.min_range = 4.0f;
  range.max_range = 200.0f;
  RpPatchSetDefaultLODCallBackRange(&range);
  return instance;
}

void *PatchEngineClose(void *instance, RwInt32 offset, RwInt32 size)
{
  if (--_rpPatchGlobals.module_num_instances == 0) {
    rpPatchPipelinesDestroy();
    RwFreeListDestroy(_rpPatchGlobals.atomic_free_list);
    _rpPatchGlobals.atomic_free_list = NULL;
    RwFreeListDestroy(_rpPatchGlobals.geometry_free_list);
    _rpPatchGlobals.geometry_free_list = NULL;
  }
  return instance;
}

/* Default LOD callback: the refinement falls linearly with the camera-space depth of the atomic, from max_lod at
   min_range to min_lod at max_range. */
RwInt32 rpPatchLODAtomicDefaultSelectLOD(void *atomic, void *user_data)
{
  PatchEngineData *data = PATCH_ENGINE_DATA();
  RwMatrixFull view;
  RwReal lod;

  RwMatrixMultiply(&view, RwFrameGetLTM(RpAtomic_object_object_parent(atomic)), RwCamera_viewMatrix(RwCameraGetCurrentCamera()));
  if (view.pos.z >= data->range.max_range) {
    return data->range.min_lod;
  }
  if (view.pos.z <= data->range.min_range) {
    return data->range.max_lod;
  }
  lod = (view.pos.z - data->range.min_range) * data->lod_slope + data->range.max_lod;
  return RwFastRealToUInt32_r(lod);
}

RwBool RpPatchSetDefaultLODCallBackRange(RpPatchLODRange *range)
{
  PatchEngineData *data = PATCH_ENGINE_DATA();
  union { __int64 wide; RwUInt32 half[2]; } min_lod;      /* the exe converts min_lod with a 64 bit fild: exact also */
                                                          /* under single precision (cl 13 would fild + add 2^32, two roundings) */
  data->range = *range;
  min_lod.half[0] = range->min_lod;
  min_lod.half[1] = 0;
  /* one expression: the exe multiplies with the unrounded (x87 register) reciprocal, not the stored float */
  data->lod_slope = ((RwReal)min_lod.wide - range->max_lod)
                  * (data->inv_range = 1.0f / (range->max_range - range->min_range));
  return TRUE;
}

/* Builds the patch pipelines (only the generic one exists in this build) and notes the device's T&L and vertex
   shader support.  The type argument is not used. */
RwBool _rpPatchPipelinesCreate(RwUInt32 type)
{
  D3D9Caps *caps;

  _rpPatchGlobals.pipelines[0] = NULL;
  _rpPatchGlobals.pipelines[1] = NULL;
  _rpPatchGlobals.pipelines[2] = NULL;
  _rpPatchGlobals.pipelines[3] = NULL;
  _rpPatchGlobals.pipelines[0] = _rwD3D9PatchPipelineCreateGeneric();
  _rpPatchGlobals.pipelines[1] = stub_return_zero();
  _rpPatchGlobals.pipelines[2] = stub_return_zero();
  _rpPatchGlobals.pipelines[3] = stub_return_zero();

  caps = RwD3D9GetCaps();
  _rpPatchGlobals.hardware_tnl = caps->dev_caps_byte2 & 1;
  if (_rpPatchGlobals.hardware_tnl && (caps->vertex_shader_version & 0xffff) >= 0x101) {
    _rpPatchGlobals.vertex_shaders = TRUE;
  } else {
    _rpPatchGlobals.vertex_shaders = FALSE;
  }
  return TRUE;
}

RwBool rpPatchPipelinesDestroy(void)
{
  void **pipeline;

  for (pipeline = _rpPatchGlobals.pipelines; pipeline < _rpPatchGlobals.pipelines + rpPATCHPIPELINE_COUNT; pipeline++) {
    if (*pipeline != NULL) {
      rxPipelineDestroy(*pipeline);
      *pipeline = NULL;
    }
  }
  return TRUE;
}

/* Pipeline node init of the patch all-in-one node: the default D3D9 render callback, no instance callbacks. */
RwBool D3D9PatchPipelineNodeInitFn(RxPipelineNodePart *node)
{
  D3D9InstanceNodeData *data;

  if (node == NULL) {
    return FALSE;
  }
  data = node->private_data;
  data->active = TRUE;
  data->instance_callback = NULL;
  data->reinstance_callback = NULL;
  data->render_callback = (void *)rxD3D9DefaultRenderCallback;
  return TRUE;
}

void *_rwD3D9PatchPipelineCreateGeneric(void)
{
  RxPipelinePart *pipeline = RxPipelineCreate();

  pipeline->plugin_id = rwID_PATCHPLUGIN;
  pipeline->plugin_data = 1;
  RxLockedPipeUnlock(RxLockedPipeAddFragment(RxPipelineLock(pipeline), NULL, D3D9PatchAtomicAllInOneNode, NULL));
  return pipeline;
}
