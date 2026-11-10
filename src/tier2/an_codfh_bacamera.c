/* bacamera (MFL.exe 0x004d1da0 - 0x004d2e1c): RenderWare Graphics 3.7 (Criterion Software), camera object: RwCameraSetProjection,
 * RwCameraCreate, CameraBeginUpdate (the camera's begin-update callback) and RwCameraClone.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/bacamera.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN / RWASSERT expanded (release build), RwFreeListAlloc through the engine
 * function table (+0x118), RWSRCGLOBAL(curCamera) / stdFunc[rwSTANDARDCAMERABEGINUPDATE] by their byte offsets (+0 / +0x4c),
 * rwObjectHasFrameInitialize / rwMatrixInitialize / RwCameraSetFrame / RwCameraSetRaster / RWERROR expanded, the RwCamera
 * fields declared here by the exe's RW 3.7 offsets with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rwCAMERA 4
#define rwPERSPECTIVE 1
#define rwPARALLEL 2
#define rwCAMERA_HINT 0x30005               /* rwMEMHINTDUR_EVENT | rwID_CAMERA */
#define rwID_COREPLUGIN 1
#define E_RW_BADPARAM 0x80000003

typedef struct RwV2dCam {
    RwReal x, y;
} RwV2dCam;
typedef struct RwCameraHead {               /* RwCamera (RW 3.7 layout of the exe) */
    RwUInt8 type, subType, flags, privateFlags;
    void *parent;                           /* object.parent: the frame */
    RwUInt8 lFrame[8];                      /* RwLLLink */
    void *sync;                             /* RwObjectHasFrameSyncFunction */
    RwInt32 projectionType;
    void *beginUpdate;
    void *endUpdate;
    RwUInt8 viewMatrix[0xc];
    RwUInt32 viewMatrixFlags;               /* viewMatrix.flags */
    RwUInt8 viewMatrixRest[0x30];
    void *frameBuffer;                      /* RwRaster * */
    void *zBuffer;
    RwV2dCam viewWindow;
    RwV2dCam recipViewWindow;
    RwV2dCam viewOffset;
    RwReal nearPlane;
    RwReal farPlane;
    RwReal fogPlane;
} RwCameraHead;
typedef struct RwErrorCode {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwErrorCode;
RW_LAYOUT_ASSERT(rw_assert_camera_matrix, RW_OFFSETOF(RwCameraHead, viewMatrixFlags) == 0x2c && RW_OFFSETOF(RwCameraHead, frameBuffer) == 0x60);
RW_LAYOUT_ASSERT(rw_assert_camera_planes, RW_OFFSETOF(RwCameraHead, viewWindow) == 0x68 && RW_OFFSETOF(RwCameraHead, fogPlane) == 0x88);

typedef void *(*RwFreeListAllocFn)(void *freeList, RwUInt32 hint);
typedef RwBool (*RwStandardFn)(void *pOut, void *pInOut, RwInt32 nI);

/*007d2128*/ extern void *RwEngineInstance;
/*007d1ff0*/ extern RwInt32 cameraModule;                    /* RwModuleInfo: globalsOffset first */
/*0062e534*/ extern RwUInt8 cameraTKList[];
/*004d1c00*/ extern void CameraUpdateZShiftScale(RwCameraHead *camera);
/*004d0f70*/ extern void *RwFrameUpdateObjects(void *frame);
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(void *reg, void *object);
/*004e9d70*/ extern RwBool _rwPluginRegistryCopyObject(void *reg, void *dstObject, const void *srcObject);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *object, void *frame);
/*004e9db0*/ extern RwBool _rwFrameSyncDirty(void);
/*0050b5e0*/ extern void _rwPipeInitForCamera(const void *camera);
/*004d1f70*/ extern void *CameraSync(void *type);
/*004d2d10*/ extern void *CameraEndUpdate(void *camera);
/*004ea160*/ extern RwErrorCode *RwErrorSet(RwErrorCode *code);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
void *CameraBeginUpdate(RwCameraHead *camera);

#define RwFreeListAlloc(fl, h) (*(RwFreeListAllocFn *)((RwUInt8 *)RwEngineInstance + 0x118))((fl), (h))      /* memoryFuncs.rwfreelistalloc */
#define RWCAMERAGLOBAL_FREELIST (*(void **)((RwUInt8 *)RwEngineInstance + cameraModule))
#define RWSRCGLOBAL_CURCAMERA (*(void **)RwEngineInstance)
#define RWSRCGLOBAL_CAMERABEGINUPDATE (*(RwStandardFn *)((RwUInt8 *)RwEngineInstance + 0x4c))      /* stdFunc[rwSTANDARDCAMERABEGINUPDATE] */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwCameraHead *RwCameraSetProjection(RwCameraHead *camera, RwInt32 projection)
{
    switch (projection)
    {
        case (rwPARALLEL):
        case (rwPERSPECTIVE):
            {
                void *frame;

                camera->projectionType = projection;

                /* Force the view matrix to be rebuilt */
                frame = camera->parent;
                if (frame)
                {
                    RwFrameUpdateObjects(frame);
                }

                /* Update the Z transformation */
                CameraUpdateZShiftScale(camera);
                return camera;
            }
        default:
            {
                /* Bad parameter type */
                RwErrorCode err;
                err.pluginID = rwID_COREPLUGIN;
                err.errorCode = _rwerror(E_RW_BADPARAM, "Invalid projection type specified");
                RwErrorSet(&err);
                return NULL;
            }
    }
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwCameraHead *RwCameraCreate(void)
{
    RwCameraHead *camera = (RwCameraHead *)RwFreeListAlloc(RWCAMERAGLOBAL_FREELIST, rwCAMERA_HINT);

    if (!camera)
    {
        return NULL;
    }

    /* If we add this to a world, the sync function turns into something else (rwObjectHasFrameInitialize) */
    camera->type = rwCAMERA;
    camera->subType = 0;
    camera->flags = 0;
    camera->privateFlags = 0;
    camera->parent = NULL;
    camera->sync = (void *)CameraSync;

    /* Begin/end camera update functions - must happen before plugin init */
    camera->beginUpdate = (void *)CameraBeginUpdate;
    camera->endUpdate = (void *)CameraEndUpdate;

    /* Set up the defaults for the camera */
    camera->viewWindow.x = camera->viewWindow.y = 1.0f;
    camera->recipViewWindow.x = camera->recipViewWindow.y = 1.0f;
    camera->viewOffset.x = camera->viewOffset.y = 0.0f;

    /* Clipping planes */
    camera->nearPlane = (RwReal)((0.05));
    camera->farPlane = (RwReal)((10.0));
    camera->fogPlane = (RwReal)((5.0));

    /* Render destination rasters */
    camera->frameBuffer = NULL;
    camera->zBuffer = NULL;

    /* Set up projection type */
    camera->projectionType = rwPERSPECTIVE;

    /* We can set up the Z buffer mapping now */
    CameraUpdateZShiftScale(camera);

    /* Initialize the view matrix (rwMatrixInitialize(&camera->viewMatrix, 0)) */
    camera->viewMatrixFlags = 0;

    /* Initialize the plugin memory */
    _rwPluginRegistryInitObject(cameraTKList, camera);
    return camera;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *CameraBeginUpdate(RwCameraHead *camera)
{
    /* OK, we are go... */
    RWSRCGLOBAL_CURCAMERA = camera;

    /* Sync up all of the objects which have moved, before we call the device */
    _rwFrameSyncDirty();

    /* Set context */
    if (RWSRCGLOBAL_CAMERABEGINUPDATE(NULL, camera, 0))
    {
        _rwPipeInitForCamera(camera);

        /* Rendering mode */
        return camera;
    }
    /* Couldn't push context */
    return NULL;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwCameraHead *RwCameraClone(RwCameraHead *camera)
{
    RwCameraHead *oldCamera = camera;
    RwCameraHead *newCamera;

    /* Create the camera */
    if (!(newCamera = RwCameraCreate()))
    {
        return NULL;
    }

    /* Link to the original frame. */
    _rwObjectHasFrameSetFrame(newCamera, oldCamera->parent);

    /* Begin/end camera update functions - must happen before plugin init */
    newCamera->beginUpdate = oldCamera->beginUpdate;
    newCamera->endUpdate = oldCamera->endUpdate;

    /* Set up the defaults for the camera */
    newCamera->viewWindow.x = oldCamera->viewWindow.x;
    newCamera->viewWindow.y = oldCamera->viewWindow.y;

    newCamera->recipViewWindow.x = oldCamera->recipViewWindow.x;
    newCamera->recipViewWindow.y = oldCamera->recipViewWindow.y;

    newCamera->viewOffset.x = oldCamera->viewOffset.x;
    newCamera->viewOffset.y = oldCamera->viewOffset.y;

    /* Clipping planes */
    newCamera->nearPlane = oldCamera->nearPlane;
    newCamera->farPlane = oldCamera->farPlane;
    newCamera->fogPlane = oldCamera->fogPlane;

    /* Render destination rasters */
    newCamera->frameBuffer = oldCamera->frameBuffer;
    newCamera->zBuffer = oldCamera->zBuffer;

    /* Set up projection type */
    newCamera->projectionType = oldCamera->projectionType;

    /* We can set up the Z buffer mapping now */
    CameraUpdateZShiftScale(newCamera);

    /* Copy the camera's plugin data */
    _rwPluginRegistryCopyObject(cameraTKList, newCamera, oldCamera);
    return newCamera;
}
