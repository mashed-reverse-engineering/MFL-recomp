/* RwEngineTerm (MFL.exe 004d3400). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/badevice.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the debug-only RWCRTCHECKMEMORY /
 * RWCRTHEAPDIFFERENCESINCE lines and a stray call of a removed debug hook dropped (MFL.exe calls only _rwPluginRegistryClose and
 * _rwMemoryClose), release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for
 * the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"

extern void         _rwMemoryClose(void);
extern RwBool
_rwPluginRegistryClose(void);
extern RwUInt32     engineInstancesOpened;
/* TIER2: unvalidated authority=diffemu */
RwBool RwEngineTerm(void)
{
    RwBool result;

    result = (0 == engineInstancesOpened);

    if (result)
    {
        _rwPluginRegistryClose();
        _rwMemoryClose();

        RWSRCGLOBAL(engineStatus) = rwENGINESTATUSIDLE;
    }

    return (result);
}
