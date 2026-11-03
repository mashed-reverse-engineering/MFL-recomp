/* RwaCoreUpdateFinish (MFL.exe 0x005b6680): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwabase.c, function RwaCoreUpdateFinish), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F140 -> rwa_obj_005bace0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6680 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt32 dword_5069B4;
extern RwUInt32 dword_5069B8;
extern rwaInstance *sub_23F2F0(void);
extern rwaInstance *rwa_obj_005bace0(rwaInstance *obj, RwBool skipSelf);

void
RwaCoreUpdateFinish(void)
{
    rwa_obj_005bace0(sub_23F2F0(), FALSE);
    dword_5069B8 = 0;
    dword_5069B4++;
}
