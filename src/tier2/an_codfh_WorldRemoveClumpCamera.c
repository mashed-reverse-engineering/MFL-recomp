/* WorldRemoveClumpCamera (MFL.exe 004f48a0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpWorldRemoveCamera inlined as in MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional
 * compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared
 * extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
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
#define WORLDCAMERAEXTFROMCAMERA(camera) ((RpWorldCameraExt *)(((RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwCamera * WorldRemoveClumpCamera(RwCamera * camera, void *data)
{
    RpWorld * const     world = (RpWorld *)data;
    RpWorldCameraExt   *cameraExt;

    cameraExt = WORLDCAMERAEXTFROMCAMERA(camera);

    if ( cameraExt && (world == cameraExt->world))
    {
        /* RpWorldRemoveCamera(world, camera), inlined in MFL.exe: */
        if (cameraExt->world)
        {
            cameraExt->world = (RpWorld *)NULL;
            cameraExt->position = 0;
        }
    }

    return (camera);
}
