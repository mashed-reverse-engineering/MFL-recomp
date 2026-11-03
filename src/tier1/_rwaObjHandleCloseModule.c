/* _rwaObjHandleCloseModule (MFL.exe 0x005b7400): RenderWare Audio (Criterion Software), rwa.dll 0.97
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobject.c, function sub_23F9A0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F9A0 -> _rwaObjHandleCloseModule; sub_2438E0 -> rwa_unit243670_005beec0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b7400 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaHandleFromLink(_link) ((RwaObjHandle *)((RwUInt8 *)(_link) - 8))
extern rwaLink dword_506A60;
extern RwUInt8 unk_506A68[0x24];
extern RwUInt32 dword_506A8C;
extern void rwa_unit243670_005beec0(void *freeList);
extern void DSPResource_Release(RwaObjHandle *handle, rwaReleaseParams *params);

void
_rwaObjHandleCloseModule(void)
{
    while (dword_506A60.next != &dword_506A60)
    {
        DSPResource_Release(rwaHandleFromLink(dword_506A60.next), NULL);
    }

    rwa_unit243670_005beec0(unk_506A68);
    dword_506A8C = 0;
}
