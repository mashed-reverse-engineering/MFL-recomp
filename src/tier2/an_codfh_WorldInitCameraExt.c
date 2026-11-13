/* WorldInitCameraExt (MFL.exe 004f3920). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldCameraExt, world) == 0xc);      /* MFL.exe 0x4f395e: mov [ecx+0xc],edx (world = NULL) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpWorldCameraExt, oldBeginUpdate) == 0x10);      /* MFL.exe 0x4f3937 / 0x4f393a: camera->beginUpdate [eax+0x18] saved to [ecx+0x10] */
RW_LAYOUT_ASSERT(rw37_own_2, RW_OFFSETOF(RpWorldCameraExt, oldSync) == 0x18);      /* MFL.exe 0x4f3943 / 0x4f3946: object.sync [eax+0x10] saved to [ecx+0x18] */
#define WORLDCAMERAEXTFROMCAMERA(camera) ((RpWorldCameraExt *)(((RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
extern RwCamera * WorldCameraBeginUpdate(RwCamera * camera);
extern RwCamera * WorldCameraEndUpdate(RwCamera * camera);
extern RwObjectHasFrame * WorldCameraSync(RwObjectHasFrame * object);
/* TIER2: unvalidated authority=diffemu */
void * WorldInitCameraExt(void *object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    RpWorldCameraExt   *cameraExt = WORLDCAMERAEXTFROMCAMERA(object);
    RwCamera           *camera = (RwCamera *) object;

    cameraExt->frustumSectors = (RpWorldSector **)NULL;
    cameraExt->space = 0;
    cameraExt->position = 0;

    cameraExt->oldBeginUpdate = camera->beginUpdate;
    cameraExt->oldEndUpdate = camera->endUpdate;
    cameraExt->oldSync = camera->object.sync;
    camera->object.sync = WorldCameraSync;
    camera->beginUpdate = WorldCameraBeginUpdate;
    camera->endUpdate = WorldCameraEndUpdate;

    cameraExt->world = (RpWorld *)NULL;

    return (object);
}
