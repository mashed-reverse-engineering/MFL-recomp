/* RwEngineClose (MFL.exe 004d31d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/badevice.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the debug-only RWNOTIFY /
 * RWCRTCHECKMEMORY lines dropped (empty in the release build), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN ->
 * return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions
 * MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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
/* ---- end of the generated declarations ---- */

extern RwBool _rwDeviceSystemRequest(RwDevice *d, RwInt32 id, void *out,
                                 void *inOut, RwInt32 numIn);
extern RwUInt32     engineInstancesOpened;
extern void               *RwEngineInstance;
extern RwGlobals    staticGlobals;
extern RwBool _rwDeviceSystemRequest(RwDevice * device, RwInt32 requestID, void *pOut, void *pInOut, RwInt32 numIn);
/* TIER2: unvalidated authority=diffemu */
RwBool RwEngineClose(void)
{
    RwBool              result = FALSE;
    const RwInt32       numRequest = 0;
    RwDevice           *target = (RwDevice *)NULL;

    target = &RWSRCGLOBAL(dOpenDevice);

    result = _rwDeviceSystemRequest(target, rwDEVICESYSTEMCLOSE,
                                    NULL, NULL, numRequest);

    if (result)
    {
        void               *instance = NULL;

        instance = RwEngineInstance;
        RwEngineInstance = (void *) (&staticGlobals);

        memcpy(RwEngineInstance, instance, sizeof(RwGlobals));

        RwFree(instance);
        instance = NULL;

        engineInstancesOpened--;
        RWSRCGLOBAL(engineStatus) = rwENGINESTATUSINITED;
    }
    return (result);
}
