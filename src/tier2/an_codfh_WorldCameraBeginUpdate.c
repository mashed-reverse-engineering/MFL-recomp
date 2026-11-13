/* WorldCameraBeginUpdate (MFL.exe 004f3970). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldCameraExt, world) == 0xc);      /* MFL.exe 0x4f3986: mov edx,[eax+0xc] (cameraExt->world -> curWorld) */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(RpWorldCameraExt, oldBeginUpdate) == 0x10);      /* MFL.exe 0x4f3997: jmp [eax+0x10] */
#define WORLDCAMERAEXTFROMCAMERA(camera) ((RpWorldCameraExt *)(((RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
/* TIER2: unvalidated authority=diffemu */
RwCamera * WorldCameraBeginUpdate(RwCamera * camera)
{
    RpWorldCameraExt   *cameraExt = WORLDCAMERAEXTFROMCAMERA(camera);

    RWSRCGLOBAL(curWorld) = (void *) cameraExt->world;

    RWSRCGLOBAL(renderFrame)++;

    return (cameraExt->oldBeginUpdate(camera));
}
