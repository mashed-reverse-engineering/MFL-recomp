/* WorldCopyCameraExt (MFL.exe 004f3c50). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpWorldAddCamera inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
#define RwCameraGetFrameMacro(_camera) ((RwFrame *)rwObjectGetParent((_camera)))
#define RwCameraGetFrame(_camera) RwCameraGetFrameMacro(_camera)
/* ---- end of the generated declarations ---- */

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
#define WORLDCAMERAEXTFROMCONSTCAMERA(camera) ((const RpWorldCameraExt *)(((const RwUInt8 *)(camera)) + cameraExtOffset))
extern RwInt32      cameraExtOffset;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpWorldCameraExt, space) == 4 && RW_OFFSETOF(RpWorldCameraExt, position) == 8 && RW_OFFSETOF(RpWorldCameraExt, world) == 0xc);      /* MFL.exe 0x4f3c67..0x4f3c6f: [ext] / [ext+4] / [ext+8] = 0, src world [ext+0xc]; 0x4f3c8f dst world [ext+0xc] */

/* TIER2: unvalidated authority=diffemu */
void * WorldCopyCameraExt(void *dstObject, const void *srcObject, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    RpWorldCameraExt   *dstCameraExt =
        WORLDCAMERAEXTFROMCAMERA(dstObject);
    const RpWorldCameraExt *srcCameraExt =
        WORLDCAMERAEXTFROMCONSTCAMERA(srcObject);

    dstCameraExt->frustumSectors = (RpWorldSector **)NULL;
    dstCameraExt->space = 0;
    dstCameraExt->position = 0;

    if (srcCameraExt->world)
    {
        /* RpWorldAddCamera(srcCameraExt->world, (RwCamera *) dstObject), inlined in MFL.exe: frame objects resynced, then the world set */
        RwCamera           *camera = (RwCamera *) dstObject;
        RpWorld            *world = srcCameraExt->world;

        if (RwCameraGetFrame(camera))
        {
            RwFrameUpdateObjects(RwCameraGetFrame(camera));
        }
        WORLDCAMERAEXTFROMCAMERA(camera)->world = world;
    }

    return (dstObject);
}
