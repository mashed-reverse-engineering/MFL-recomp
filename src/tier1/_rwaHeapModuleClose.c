/* _rwaHeapModuleClose (MFL.exe 0x005beb30): RenderWare Audio (Criterion Software), rwa.dll 0.968
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_244220.c, function sub_244510), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2438E0 -> rwa_unit243670_005beec0; sub_244510 -> _rwaHeapModuleClose), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005beb30 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaPoolFromLink(_link) ((rwaPool *)((RwUInt8 *)(_link) - 0x14))
extern rwaLink dword_5070BC;
extern RwUInt8 unk_5070C4[0x24];
extern RwUInt32 dword_5070E8;
extern void rwa_unit243670_005beec0(void *freeList);
extern void sub_2443F0(rwaPool *pool);

void
_rwaHeapModuleClose(void)
{
    while (dword_5070BC.next != &dword_5070BC)
    {
        sub_2443F0(rwaPoolFromLink(dword_5070BC.next));
    }

    rwa_unit243670_005beec0(unk_5070C4);
    dword_5070E8 = 0;
}
