/* RwEngineStop (MFL.exe 004d30f0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/badevice.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the debug-only RWNOTIFY /
 * RWCRTCHECKMEMORY lines dropped (empty in the release build), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;
struct RwPluginRegistry
{
    RwInt32 sizeOfStruct;
    RwInt32 origSizeOfStruct;
    RwInt32 maxSizeOfStruct;
    RwInt32 staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};
extern const
RwPluginRegistry *_rwPluginRegistryDeInitObject(const RwPluginRegistry * reg,
                                               void *object);
enum RwCoreDeviceSystemFn
{
    rwDEVICESYSTEMOPEN = 0x00,
    rwDEVICESYSTEMCLOSE,
    rwDEVICESYSTEMSTART,
    rwDEVICESYSTEMSTOP,
    rwDEVICESYSTEMREGISTER,
    rwDEVICESYSTEMGETNUMMODES,
    rwDEVICESYSTEMGETMODEINFO,
    rwDEVICESYSTEMUSEMODE,
    rwDEVICESYSTEMFOCUS,
    rwDEVICESYSTEMINITPIPELINE,
    rwDEVICESYSTEMGETMODE,
    rwDEVICESYSTEMSTANDARDS,
    rwDEVICESYSTEMGETTEXMEMSIZE,
    rwDEVICESYSTEMGETNUMSUBSYSTEMS,
    rwDEVICESYSTEMGETSUBSYSTEMINFO,
    rwDEVICESYSTEMGETCURRENTSUBSYSTEM,
    rwDEVICESYSTEMSETSUBSYSTEM,
    rwDEVICESYSTEMFINALIZESTART,
    rwDEVICESYSTEMINITIATESTOP,
    rwDEVICESYSTEMGETMAXTEXTURESIZE,
    rwDEVICESYSTEMRXPIPELINEREQUESTPIPE,
    rwDEVICESYSTEMGETMETRICBLOCK,
    rwDEVICESYSTEMGETID,
    rwDEVICESYSTEMDD = 0x1000,
    rwCOREDEVICESYSTEMFNFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCoreDeviceSystemFn RwCoreDeviceSystemFn;
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RwPluginRegistry, maxSizeOfStruct) == 0x8);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RwPluginRegistry, firstRegEntry) == 0x10);      /* an_codfh__rwPluginRegistryAddPlugin.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RwPluginRegistry, lastRegEntry) == 0x14);      /* an_codfh__rwPluginRegistryAddPlugin.c */
/* ---- end of the generated declarations ---- */

extern RwBool _rwDeviceSystemRequest(RwDevice *d, RwInt32 id, void *out,
                                 void *inOut, RwInt32 numIn);
extern RwPluginRegistry engineTKList;
extern void               *RwEngineInstance;
extern RwBool _rwDeviceSystemRequest(RwDevice * device, RwInt32 requestID, void *pOut, void *pInOut, RwInt32 numIn);
/* TIER2: unvalidated authority=diffemu */
RwBool RwEngineStop(void)
{
    RwBool result;
    const RwInt32       numRequest = 0;
    RwDevice *const device = &RWSRCGLOBAL(dOpenDevice);

    _rwDeviceSystemRequest(device,
                           rwDEVICESYSTEMINITIATESTOP, NULL, NULL,
                           numRequest);

    _rwPluginRegistryDeInitObject(&engineTKList, RwEngineInstance);

    result =
        _rwDeviceSystemRequest(device, rwDEVICESYSTEMSTOP,
                               NULL, NULL, numRequest);

    if (result)
    {
        RWSRCGLOBAL(engineStatus) = rwENGINESTATUSOPENED;
    }

    return (result);
}
