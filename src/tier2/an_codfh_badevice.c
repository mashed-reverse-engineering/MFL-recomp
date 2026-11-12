/* badevice (MFL.exe 0x004d2fa0..0x004d3740): RenderWare Graphics 3.7 (Criterion Software), engine run-up: RwEngineInit / RwEngineOpen /
 * RwEngineStart and the sub-system / video-mode queries.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/badevice.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWFUNCTION / RWASSERT / RWNOTIFY / RWCRT* / RWRETURN expanded (release build);
 * RWERROR expanded as the release build has it (_rwerror + RwErrorSet); RwMalloc / RwFree expanded to the RwGlobals memory functions;
 * the static EngineOpen and CorePluginAttach inlined where the exe has them, RwEngineRegisterPlugin expanded to
 * _rwPluginRegistryAddPlugin(&engineTKList, ...); RwEngineInit without the 3.6 file-function open / close (sub_2AB700 / nullsub_1:
 * the 3.7 exe has none) and with the exe's module order (chunk group before _rwPipeAttach); RwEngineOpen points RwEngineInstance back at
 * staticGlobals when the globals allocation fails (as the exe does); the memcpy of the globals is a struct assignment; folded callees
 * by their exe names (_rwPipeAttach = stub_return_one, _rwStringClose = nop_stub); structs declared here with compile-time layout
 * asserts (values of librw-mfl capi/rw37.h RwGlobals37 and the exe).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwDeviceHead {
    RwReal gammaCorrection;             /* 0x00 */
    RwUInt8 rest[0x34];                 /* fpSystem, zBufferNear / Far, render state / im2d functions */
} RwDeviceHead;
typedef struct RwMemoryFunctions {
    void *(*rwmalloc)(size_t size, RwUInt32 hint);
    void (*rwfree)(void *mem);
    void *(*rwrealloc)(void *mem, size_t newSize, RwUInt32 hint);
    void *(*rwcalloc)(size_t numObj, size_t sizeObj, RwUInt32 hint);
} RwMemoryFunctions;
typedef struct RwGlobals {
    void *curCamera;                    /* 0x00 */
    void *curWorld;                     /* 0x04 */
    RwUInt8 pad08[8];                   /* 0x08 renderFrame, lockedDevice */
    RwDeviceHead dOpenDevice;           /* 0x10 */
    void *stdFunc[29];                  /* 0x48 */
    RwUInt8 dirtyFrameList[8];          /* 0xbc */
    void *stringFuncs[17];              /* 0xc4 */
    RwMemoryFunctions memoryFuncs;      /* 0x108 */
    void *memoryAlloc;                  /* 0x118 free list alloc */
    void *memoryFree;                   /* 0x11c free list free */
    void *metrics;                      /* 0x120 */
    RwInt32 engineStatus;               /* 0x124 */
    RwUInt32 resArenaInitSize;          /* 0x128 */
} RwGlobals;
typedef struct RwPluginRegistry {
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    void *firstRegEntry;
    void *lastRegEntry;
} RwPluginRegistry;
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_dOpenDevice, RW_OFFSETOF(RwGlobals, dOpenDevice) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_stdFunc, RW_OFFSETOF(RwGlobals, stdFunc) == 0x48);
RW_LAYOUT_ASSERT(rw_assert_stringFuncs, RW_OFFSETOF(RwGlobals, stringFuncs) == 0xc4);
RW_LAYOUT_ASSERT(rw_assert_memoryFuncs, RW_OFFSETOF(RwGlobals, memoryFuncs) == 0x108);
RW_LAYOUT_ASSERT(rw_assert_memoryAlloc, RW_OFFSETOF(RwGlobals, memoryAlloc) == 0x118);
RW_LAYOUT_ASSERT(rw_assert_engineStatus, RW_OFFSETOF(RwGlobals, engineStatus) == 0x124);
RW_LAYOUT_ASSERT(rw_assert_resArena, RW_OFFSETOF(RwGlobals, resArenaInitSize) == 0x128);
RW_LAYOUT_ASSERT(rw_assert_globals_size, sizeof(RwGlobals) == 0x12c);
RW_LAYOUT_ASSERT(rw_assert_error_code, RW_OFFSETOF(RwError, errorCode) == 4);

/* RwEngineStatus, device requests (rwDEVICESYSTEM*), error codes, plugin IDs */
enum { rwENGINESTATUSIDLE = 0, rwENGINESTATUSINITED = 1, rwENGINESTATUSOPENED = 2, rwENGINESTATUSSTARTED = 3 };
enum { rwDEVICESYSTEMOPEN = 0x00, rwDEVICESYSTEMSTART = 0x02, rwDEVICESYSTEMSTOP = 0x03, rwDEVICESYSTEMREGISTER = 0x04,
       rwDEVICESYSTEMGETMODEINFO = 0x06, rwDEVICESYSTEMSTANDARDS = 0x0b, rwDEVICESYSTEMGETSUBSYSTEMINFO = 0x0e,
       rwDEVICESYSTEMFINALIZESTART = 0x11 };
#define rwENGINEINITNOFREELISTS 0x1
#define rwMEMHINTDUR_GLOBAL 0x40000
#define rwID_COREPLUGIN 0x01
#define E_RW_BADENGINESTATE ((RwInt32)0x80000001)
#define E_RW_NOMEM ((RwInt32)0x80000013)
#define E_RW_NULLP ((RwInt32)0x80000016)

/*007d2128*/ extern RwGlobals *RwEngineInstance;
/*007d1ff8*/ extern RwGlobals staticGlobals;
/*007d2124*/ extern RwUInt32 engineInstancesOpened;
/*0062e578*/ extern RwPluginRegistry engineTKList;
#define RWSRCGLOBAL(variable) (RwEngineInstance->variable)

/*004d2e20*/ extern RwBool _rwDeviceSystemRequest(void *device, RwInt32 requestID, void *pOut, void *pInOut, RwInt32 numIn);
/*004dafd0*/ extern void *FUN_004dafd0(void);      /* _rwDeviceGetHandle */
/*004d7bf0*/ extern RwBool _rwD3D9RasterPluginAttach_004df7b0(void);      /* _rwDeviceRegisterPlugin */
/*004e9ce0*/ extern RwBool _rwPluginRegistryInitObject(RwPluginRegistry *reg, void *object);
/*004e9950*/ extern RwBool _rwPluginRegistryOpen(void);
/*004e9990*/ extern RwBool _rwPluginRegistryClose(void);
/*004e9ad0*/ extern RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry *reg, RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004dea10*/ extern RwBool RwImageSetGamma(RwReal gammaValue);
/*004ea210*/ extern RwBool _rwStringOpen(void);
/*0050b5e0*/ extern void nop_stub(void);      /* _rwStringClose (empty, folded) */
/*004e96e0*/ extern RwBool _rwMemoryOpen(const RwMemoryFunctions *memFuncs);
/*004e9800*/ extern RwBool _rwMemoryClose(void);
/*004e90a0*/ extern void FUN_004e90a0(RwBool enable);      /* _rwFreeListEnable */
/*004a34a0*/ extern RwInt32 stub_return_one(void);      /* _rwPipeAttach (returns 1, folded) */
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);
/* free list allocators and the module open / close callbacks */
/*004d3740*/ extern void *MallocWrapper(void *fl, RwUInt32 hint);
/*004d3760*/ extern void *FreeWrapper(void *fl, void *pData);
/*004e9340*/ extern void *_rwFreeListAllocReal(void *fl, RwUInt32 hint);
/*004e9460*/ extern void *_rwFreeListFreeReal(void *fl, void *pData);
/*004ea110*/ extern void *rwErrorOpen(void *object, RwInt32 offset, RwInt32 size);
/*004ea150*/ extern void *rwErrorClose(void *object, RwInt32 offset, RwInt32 size);
/*004d5770*/ extern void *rwVectorOpen(void *object, RwInt32 offset, RwInt32 size);
/*004d5700*/ extern void *rwVectorClose(void *object, RwInt32 offset, RwInt32 size);
/*004eacd0*/ extern void *gap_4eacd0(void *object, RwInt32 offset, RwInt32 size);      /* _rwColorOpen */
/*004eace0*/ extern void *gap_4eace0(void *object, RwInt32 offset, RwInt32 size);      /* _rwColorClose */
/*004d3b40*/ extern void *rwMatrixOpen(void *object, RwInt32 offset, RwInt32 size);
/*004d3b00*/ extern void *rwMatrixClose(void *object, RwInt32 offset, RwInt32 size);
/*004d08d0*/ extern void *LAB_004d08d0(void *object, RwInt32 offset, RwInt32 size);      /* _rwFrameOpen */
/*004d0950*/ extern void *LAB_004d0950(void *object, RwInt32 offset, RwInt32 size);      /* _rwFrameClose */
/*004dbde0*/ extern void *LAB_004dbde0(void *object, RwInt32 offset, RwInt32 size);      /* _rwStreamModuleOpen */
/*004dbe40*/ extern void *rwStreamModuleClose(void *object, RwInt32 offset, RwInt32 size);
/*004d1b10*/ extern void *LAB_004d1b10(void *object, RwInt32 offset, RwInt32 size);      /* _rwCameraOpen */
/*004d1ad0*/ extern void *rwCameraClose(void *object, RwInt32 offset, RwInt32 size);
/*004dd1b0*/ extern void *rwImageOpen(void *object, RwInt32 offset, RwInt32 size);
/*004dd410*/ extern void *rwImageClose(void *object, RwInt32 offset, RwInt32 size);
/*004d7a70*/ extern void *rwRasterOpen(void *object, RwInt32 offset, RwInt32 size);
/*004d7a30*/ extern void *rwRasterClose(void *object, RwInt32 offset, RwInt32 size);
/*004d60f0*/ extern void *rwTextureOpen(void *object, RwInt32 offset, RwInt32 size);
/*004d5f90*/ extern void *rwTextureClose(void *object, RwInt32 offset, RwInt32 size);
/*004e3b00*/ extern void *rwRenderPipelineOpen(void *object, RwInt32 offset, RwInt32 size);
/*004e3b20*/ extern void *rwRenderPipelineClose(void *object, RwInt32 offset, RwInt32 size);
/*004eac40*/ extern void *LAB_004eac40(void *object, RwInt32 offset, RwInt32 size);      /* _rwChunkGroupOpen */
/*004eaca0*/ extern void *rwChunkGroupClose(void *object, RwInt32 offset, RwInt32 size);
/*004dd100*/ extern void *rwIm3DOpen(void *object, RwInt32 offset, RwInt32 size);
/*004dd0c0*/ extern void *rwIm3DClose(void *object, RwInt32 offset, RwInt32 size);
/*004ea720*/ extern void *rwResourcesOpen(void *object, RwInt32 offset, RwInt32 size);
/*004ea810*/ extern void *rwResourcesClose(void *object, RwInt32 offset, RwInt32 size);

#define REGISTER(size, id, open, close) _rwPluginRegistryAddPlugin(&engineTKList, (size), (id), (void *)(open), (void *)(close), NULL)

/* TIER2: validated 2026-11-10 authority=diffemu */
void *RwEngineGetSubSystemInfo(void *subSystemInfo, RwInt32 subSystemIndex)
{
    if (!_rwDeviceSystemRequest(&RWSRCGLOBAL(dOpenDevice), rwDEVICESYSTEMGETSUBSYSTEMINFO, subSystemInfo, NULL, subSystemIndex))
    {
        /* Device doesn't know about rendering sub systems */
        subSystemInfo = NULL;
    }
    return subSystemInfo;
}

/* TIER2: validated 2026-11-10 authority=diffemu */
void *RwEngineGetVideoModeInfo(void *modeinfo, RwInt32 modeIndex)
{
    if (!_rwDeviceSystemRequest(&RWSRCGLOBAL(dOpenDevice), rwDEVICESYSTEMGETMODEINFO, modeinfo, NULL, modeIndex))
    {
        /* Device doesn't know about modes */
        modeinfo = NULL;
    }
    return modeinfo;
}

/* TIER2: validated 2026-11-10 authority=diffemu */
RwBool RwEngineStart(void)
{
    RwDeviceHead *const device = &RWSRCGLOBAL(dOpenDevice);

    if (_rwDeviceSystemRequest(device, rwDEVICESYSTEMSTART, NULL, NULL, 0))
    {
        /* Initialize the plugin memory */
        if (_rwPluginRegistryInitObject(&engineTKList, RwEngineInstance))
        {
            /* Now we can set up the gamma correction - we have an open image module */
            RwImageSetGamma(RWSRCGLOBAL(dOpenDevice).gammaCorrection);

            /* Get the device to finalise its objects */
            _rwDeviceSystemRequest(device, rwDEVICESYSTEMFINALIZESTART, NULL, NULL, 0);

            RWSRCGLOBAL(engineStatus) = rwENGINESTATUSSTARTED;
            return 1;      /* TRUE */
        }
        _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTOP, NULL, NULL, 0);
    }
    return 0;      /* FALSE */
}

/* TIER2: validated 2026-11-10 authority=diffemu */
RwBool RwEngineOpen(void *initParams)
{
    RwBool result;
    RwError error;

    if (!RwEngineInstance)
    {
        /* Bootstrap the library by giving it some static globals */
        RwEngineInstance = &staticGlobals;
    }

    result = (RWSRCGLOBAL(engineStatus) == rwENGINESTATUSINITED);
    if (result)
    {
        result = (NULL != initParams);
        if (result)
        {
            /* Attempt to get the device block */
            void *const device = FUN_004dafd0();

            result = (NULL != device);
            if (result)
            {
                /* EngineOpen: allocate the globals first, the device grabs references to them */
                RwEngineInstance = (RwGlobals *)RWSRCGLOBAL(memoryFuncs).rwmalloc(engineTKList.sizeOfStruct, rwMEMHINTDUR_GLOBAL);
                if (RwEngineInstance)
                {
                    RwGlobals *instance = RwEngineInstance;

                    /* Copy across the staticGlobals */
                    *RwEngineInstance = staticGlobals;

                    /* Register it first: pulls the device block back, pushes down the memory interface */
                    _rwDeviceSystemRequest(device, rwDEVICESYSTEMREGISTER, &RWSRCGLOBAL(dOpenDevice), &RWSRCGLOBAL(memoryFuncs), 0);

                    /* Try and start it up */
                    if (_rwDeviceSystemRequest(device, rwDEVICESYSTEMOPEN, NULL, initParams, 0))
                    {
                        /* Set up the standard functions */
                        _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTANDARDS, RWSRCGLOBAL(stdFunc), NULL, sizeof(RWSRCGLOBAL(stdFunc)) / sizeof(void *));

                        /* One more instance opened */
                        engineInstancesOpened++;
                        RWSRCGLOBAL(engineStatus) = rwENGINESTATUSOPENED;
                        return 1;      /* TRUE */
                    }

                    /* Point back at the static instance so we can blow away the allocated one */
                    RwEngineInstance = &staticGlobals;
                    *RwEngineInstance = *instance;
                    RWSRCGLOBAL(memoryFuncs).rwfree(instance);
                    return 0;      /* FALSE: failed to open the device */
                }

                /* Failed to allocate memory for the globals */
                RwEngineInstance = &staticGlobals;
                error.pluginID = rwID_COREPLUGIN;
                error.errorCode = _rwerror(E_RW_NOMEM, engineTKList.sizeOfStruct);
                RwErrorSet(&error);
                return 0;      /* FALSE */
            }
            /* else: failed to get the device handle */
        }
        else
        {
            error.pluginID = rwID_COREPLUGIN;
            error.errorCode = _rwerror(E_RW_NULLP);
            RwErrorSet(&error);
        }
    }
    else
    {
        error.pluginID = rwID_COREPLUGIN;
        error.errorCode = _rwerror(E_RW_BADENGINESTATE);
        RwErrorSet(&error);
    }
    return result;
}

/* TIER2: validated 2026-11-10 authority=diffemu */
RwBool RwEngineInit(const RwMemoryFunctions *memFuncs, RwUInt32 openFlags, RwUInt32 resArenaSize)
{
    RwBool result = 0;      /* FALSE */

    /* Bootstrap the library by giving it some static globals */
    RwEngineInstance = &staticGlobals;

    if (openFlags & rwENGINEINITNOFREELISTS)
    {
        RWSRCGLOBAL(memoryAlloc) = (void *)MallocWrapper;
        RWSRCGLOBAL(memoryFree) = (void *)FreeWrapper;
        FUN_004e90a0(0);
    }
    else
    {
        RWSRCGLOBAL(memoryAlloc) = (void *)_rwFreeListAllocReal;
        RWSRCGLOBAL(memoryFree) = (void *)_rwFreeListFreeReal;
        FUN_004e90a0(1);
    }

    RWSRCGLOBAL(resArenaInitSize) = resArenaSize;

    /* We mustn't have initted the engine previously */
    if (RWSRCGLOBAL(engineStatus) == rwENGINESTATUSIDLE)
    {
        /* The string operations first, so that the debug stuff stands a chance */
        result = _rwStringOpen();
        if (result)
        {
            result = _rwMemoryOpen(memFuncs);
            if (result)
            {
                result = _rwPluginRegistryOpen();
                if (result)
                {
                    /* CorePluginAttach: register the default modules */
                    RwInt32 state = 0;

                    state |= REGISTER(8, 0x40f, rwErrorOpen, rwErrorClose);                     /* rwID_ERRORMODULE */
                    state |= REGISTER(0x18, 0x401, rwVectorOpen, rwVectorClose);                /* rwID_VECTORMODULE */
                    state |= REGISTER(0, 0x40d, gap_4eacd0, gap_4eace0);                        /* rwID_COLORMODULE */
                    state |= REGISTER(0x18, 0x402, rwMatrixOpen, rwMatrixClose);                /* rwID_MATRIXMODULE */
                    state |= REGISTER(4, 0x403, LAB_004d08d0, LAB_004d0950);                    /* rwID_FRAMEMODULE */
                    state |= REGISTER(4, 0x404, LAB_004dbde0, rwStreamModuleClose);             /* rwID_STREAMMODULE */
                    state |= REGISTER(4, 0x405, LAB_004d1b10, rwCameraClose);                   /* rwID_CAMERAMODULE */
                    state |= REGISTER(0x220, 0x406, rwImageOpen, rwImageClose);                 /* rwID_IMAGEMODULE */
                    state |= REGISTER(0x64, 0x407, rwRasterOpen, rwRasterClose);                /* rwID_RASTERMODULE */
                    state |= REGISTER(0x34, 0x408, rwTextureOpen, rwTextureClose);              /* rwID_TEXTUREMODULE */
                    state |= REGISTER(0x60, 0x409, rwRenderPipelineOpen, rwRenderPipelineClose); /* rwID_PIPEMODULE */
                    state |= REGISTER(4, 0x412, LAB_004eac40, rwChunkGroupClose);               /* rwID_CHUNKGROUPMODULE */
                    state |= stub_return_one();                                                 /* _rwPipeAttach */
                    state |= REGISTER(0x74, 0x40a, rwIm3DOpen, rwIm3DClose);                    /* rwID_IMMEDIATEMODULE */
                    state |= REGISTER(0x28, 0x40b, rwResourcesOpen, rwResourcesClose);          /* rwID_RESOURCESMODULE */

                    /* If all are positive, we've succeeded */
                    if (state >= 0)
                    {
                        /* Give the drivers a chance to register plugins */
                        result = _rwD3D9RasterPluginAttach_004df7b0();
                        if (result)
                        {
                            RWSRCGLOBAL(engineStatus) = rwENGINESTATUSINITED;
                            return result;
                        }
                    }
                    else
                    {
                        result = 0;
                    }
                    _rwPluginRegistryClose();
                }
                _rwMemoryClose();
            }
            nop_stub();      /* _rwStringClose */
        }
    }

    /* We don't have debug macro capability yet */
    return result;
}
