/* WorldDeInitCameraExt (MFL.exe 004f3ca0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * SectorsInFrustumDeinitialise inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

typedef struct RpWorldCameraExt RpWorldCameraExt;
struct RpWorldCameraExt
{
    
    RpWorldSector     **frustumSectors;
    RwInt32             space;
    RwInt32             position;

    RpWorld            *world;

    RwCameraBeginUpdateFunc oldBeginUpdate;
    RwCameraEndUpdateFunc oldEndUpdate;

    RwObjectHasFrameSyncFunction oldSync;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldCameraExt, position) == 8 && RW_OFFSETOF(RpWorldCameraExt, oldBeginUpdate) == 0x10 && RW_OFFSETOF(RpWorldCameraExt, oldEndUpdate) == 0x14 && RW_OFFSETOF(RpWorldCameraExt, oldSync) == 0x18);      /* MFL.exe 0x4f3cc5..0x4f3ce8 (WorldDeInitCameraExt): [esi+0x8] = 0, [esi+0x10] / [esi+0x14] / [esi+0x18] restored to camera +0x18 / +0x1c / +0x10 */
#define WORLDCAMERAEXTFROMCAMERA(camera) ((RpWorldCameraExt *)(((RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
/* TIER2: unvalidated authority=diffemu */
void * WorldDeInitCameraExt(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    RpWorldCameraExt   *cameraExt = WORLDCAMERAEXTFROMCAMERA(object);
    RwCamera           *camera = (RwCamera *) object;

    /* SectorsInFrustumDeinitialise, inlined as in MFL.exe */
    if (cameraExt->frustumSectors)
    {
        RwFree(cameraExt->frustumSectors);
    }
    cameraExt->frustumSectors = (RpWorldSector **)NULL;
    cameraExt->space = 0;
    cameraExt->position = 0;

    camera->beginUpdate = cameraExt->oldBeginUpdate;
    camera->endUpdate = cameraExt->oldEndUpdate;
    camera->object.sync = cameraExt->oldSync;

    return (object);
}
