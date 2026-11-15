#include "mfl_tier2.h"
/* RenderWare skin plugin (rpskin), D3D9: rxSkinD3D9AtomicAllInOnePipelineInit (0x542cf0), the node init of the skin
   all-in-one node: the default render / lighting callbacks, and the vertex shader callbacks when the device has vertex
   shaders (_rpSkinGlobals, set by _rpSkinD3D9CreatePlainPipe).  Written from the disassembly of MFL.exe.
   G3, after GD1: the vertex shader callbacks are K4's DLL rows (referenced by name); rpD3D9SkinVertexShaderBeginCallBack
   (0x542c40) is a trap row the original references here too (tools/delink/trap_caller_allow.txt). */

typedef void (*SkinNodeCallback)(void);

typedef struct SkinAllInOneNodeData {     /* private data of the skin all-in-one node */
  SkinNodeCallback render;                /* 0x00 */
  SkinNodeCallback lighting;              /* 0x04 */
  SkinNodeCallback vs_begin;              /* 0x08 */
  SkinNodeCallback vs_lighting;           /* 0x0c */
  SkinNodeCallback vs_get_material_shader;/* 0x10 */
  SkinNodeCallback vs_mesh_render;        /* 0x14 */
  SkinNodeCallback vs_end;                /* 0x18 */
} SkinAllInOneNodeData;

typedef struct RxPipelineNodePart {       /* the RxPipelineNode field read here */
  char pad00[0x14];
  SkinAllInOneNodeData *private_data;     /* 0x14 */
} RxPipelineNodePart;

typedef struct SkinGlobalsVS {            /* _rpSkinGlobals (0x7da640), the field read here */
  char pad00[0x44];
  RwBool vertex_shaders;                  /* 0x44 */
} SkinGlobalsVS;

/*007da640*/ extern SkinGlobalsVS _rpSkinGlobals;

/*004faeb0*/ extern void rxD3D9DefaultRenderCallback(void);
/*004ff4c0*/ extern void _rwD3D9AtomicDefaultLightingCallback(void);
/*00542c40*/ extern void rpD3D9SkinVertexShaderBeginCallBack(void);
/*0050b750*/ extern void rxD3D9VertexShaderDefaultLightingCallBack(void);
/*0050b5f0*/ extern void rxD3D9VertexShaderDefaultGetMaterialShaderCallBack(void);
/*0050b610*/ extern void rxD3D9VertexShaderDefaultMeshRenderCallBack(void);
/*0050b5e0*/ extern void rxD3D9VertexShaderDefaultEndCallBack(void);

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwBool rxSkinD3D9AtomicAllInOnePipelineInit(RxPipelineNodePart *node)
{
  SkinAllInOneNodeData *data = node->private_data;

  data->render = rxD3D9DefaultRenderCallback;
  data->lighting = _rwD3D9AtomicDefaultLightingCallback;
  if (_rpSkinGlobals.vertex_shaders) {
    data->vs_begin = rpD3D9SkinVertexShaderBeginCallBack;
    data->vs_lighting = rxD3D9VertexShaderDefaultLightingCallBack;
    data->vs_get_material_shader = rxD3D9VertexShaderDefaultGetMaterialShaderCallBack;
    data->vs_mesh_render = rxD3D9VertexShaderDefaultMeshRenderCallBack;
    data->vs_end = rxD3D9VertexShaderDefaultEndCallBack;
  } else {
    data->vs_begin = NULL;
    data->vs_lighting = NULL;
    data->vs_get_material_shader = NULL;
    data->vs_mesh_render = NULL;
    data->vs_end = NULL;
  }
  return TRUE;
}
