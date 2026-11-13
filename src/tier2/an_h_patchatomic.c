#include "mfl_tier2.h"
/* RenderWare patch plugin (rppatch): the atomic extension callbacks (0x5598e0 .. 0x559a00).  Written from the
   disassembly of MFL.exe.  The LOD callback the constructor installs is in an_h_patchengine.c (a separate file, so
   its address stays the original's in diffemu). */

typedef struct RpPatchMesh RpPatchMesh;

typedef struct PatchAtomicData {          /* the patch extension of an atomic (at atomic + atomicOffset) */
  void *lod_callback;                     /* RpPatchLODCallBack: picks the refinement level */
  void *lod_user_data;
  RpPatchMesh *patch_mesh;                /* referenced, NULL for a plain atomic */
} PatchAtomicData;

typedef struct PatchGlobals {
  RwInt32 engine_offset;
  RwInt32 atomic_offset;                  /* RpAtomicRegisterPlugin result */
  RwInt32 geometry_offset;                /* RpGeometryRegisterPlugin result */
} PatchGlobals;

/*007da860*/ extern PatchGlobals _rpPatchGlobals;
/*00559bb0*/ extern RwBool rpPatchMeshDestroy(RpPatchMesh *mesh);
/*00559c30*/ extern RpPatchMesh *rpPatchMeshAddRef(RpPatchMesh *mesh);
/*00559e70*/ extern RwInt32 rpPatchLODAtomicDefaultSelectLOD(void *atomic, void *user_data);
/*0055a0c0*/ extern void *rpPatchPipelinesAttach(void *atomic, RwUInt32 type);

#define ATOMIC_PATCH_DATA(atomic) ((PatchAtomicData *)((char *)(atomic) + _rpPatchGlobals.atomic_offset))
#define GEOMETRY_PATCH_MESH(geometry) (*(RpPatchMesh **)((char *)(geometry) + _rpPatchGlobals.geometry_offset))

void *rpPatchAtomicConstructor(void *atomic, RwInt32 offset, RwInt32 size)
{
  PatchAtomicData *data = ATOMIC_PATCH_DATA(atomic);

  data->patch_mesh = NULL;
  data->lod_callback = (void *)rpPatchLODAtomicDefaultSelectLOD;
  data->lod_user_data = NULL;
  return atomic;
}

void *rpPatchAtomicDestructor(void *atomic, RwInt32 offset, RwInt32 size)
{
  PatchAtomicData *data = ATOMIC_PATCH_DATA(atomic);

  if (data->patch_mesh != NULL) {
    rpPatchMeshDestroy(data->patch_mesh);
  }
  data->patch_mesh = NULL;
  data->lod_callback = NULL;
  data->lod_user_data = NULL;
  return atomic;
}

void *rpPatchAtomicCopy(void *dst, const void *src, RwInt32 offset, RwInt32 size)
{
  const PatchAtomicData *from = ATOMIC_PATCH_DATA(src);
  PatchAtomicData *to = ATOMIC_PATCH_DATA(dst);

  to->patch_mesh = from->patch_mesh;
  to->lod_callback = from->lod_callback;
  to->lod_user_data = from->lod_user_data;
  if (to->patch_mesh != NULL) {
    rpPatchMeshAddRef(to->patch_mesh);
  }
  return dst;
}

/* Stream "always" callback: an atomic whose geometry carries a patch mesh takes a reference and the patch pipeline. */
RwBool rpPatchAtomicAlways(void *atomic, RwInt32 offset, RwInt32 size)
{
  RpPatchMesh *mesh = GEOMETRY_PATCH_MESH(RpAtomic_geometry(atomic));

  if (mesh != NULL) {
    ATOMIC_PATCH_DATA(atomic)->patch_mesh = mesh;
    rpPatchMeshAddRef(mesh);
    rpPatchPipelinesAttach(atomic, 0);
  }
  return TRUE;
}

/* Stream rights callback: as above, with the pipeline type from the rights chunk. */
RwBool rpPatchAtomicRights(void *atomic, RwInt32 offset, RwInt32 size, RwUInt32 extra_data)
{
  RpPatchMesh *mesh = GEOMETRY_PATCH_MESH(RpAtomic_geometry(atomic));

  if (mesh != NULL) {
    ATOMIC_PATCH_DATA(atomic)->patch_mesh = mesh;
    rpPatchMeshAddRef(mesh);
    rpPatchPipelinesAttach(atomic, extra_data);
  }
  return TRUE;
}
