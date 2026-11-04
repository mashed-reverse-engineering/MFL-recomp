/* RwaVoiceManagerUnregister (MFL.exe 0x005b77f0): RenderWare Audio (Criterion Software), rwa.dll 0.971
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavoicemanager.c, function DSPResourceMgr_Shutdown), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (DSPResourceMgr_Shutdown -> RwaVoiceManagerUnregister), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * P7c: byte-identical to MFL.exe (tools/cmpfn.py, cl 13.00 and 13.10; MFL-specific changes are noted in the code). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt32 dword_5069D8;
extern void **dword_5069D0;
extern RwaObjHandle *dword_5069DC;
extern RwInt32 dword_5069E0;
extern RwInt32 sub_23F940(RwaObjHandle *handle, RwInt32 param, RwInt32 type, void *value);
extern void DSPResource_Release(void *resource, RwInt32 a);
extern void sub_2421B0(void *data);

static __forceinline void
handleFree(RwaObjHandle *handle)
{
    DSPResource_Release(handle, 0);
    sub_2421B0(handle);
}

void
RwaVoiceManagerUnregister(void)
{
    RwInt32         i;

    sub_23F940(dword_5069DC, 5, 1, NULL);

    for (i = 0; i < dword_5069E0; i++)
    {
        DSPResource_Release(dword_5069D0[i], 0);
    }

    handleFree(dword_5069DC);

    dword_5069D8 = 0;
    dword_5069E0 = 0;
}
